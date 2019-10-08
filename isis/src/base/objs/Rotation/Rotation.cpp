#include "SpiceRotation.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iomanip>
#include <string>
#include <vector>

#include <QDebug>
#include <QString>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>



#include "BasisFunction.h"
#include "IException.h"
#include "IString.h"
#include "LeastSquares.h"
#include "LineEquation.h"
#include "NaifStatus.h"
#include "PolynomialUnivariate.h"
#include "Quaternion.h"
#include "Table.h"
#include "TableField.h"

// Declarations for bindings for Naif Spicelib routines that do not have
// a wrapper
extern int refchg_(integer *frame1, integer *frame2, doublereal *et,
                   doublereal *rotate);
extern int frmchg_(integer *frame1, integer *frame2, doublereal *et,
                   doublereal *rotate);
extern int invstm_(doublereal *mat, doublereal *invmat);
// Temporary declarations for bindings for Naif supportlib routines

// These three declarations should be removed once supportlib is in Isis3
extern int ck3sdn(double sdntol, bool avflag, int *nrec,
                  double *sclkdp, double *quats, double *avvs,
                  int nints, double *starts, double *dparr,
                  int *intarr);

namespace Isis {
  /**
   * Return the camera angles at the center time of the observation
   *
   * @return @b vector<double> Camera angles at center time
   */
  std::vector<double> Rotation::GetCenterAngles() {
    // Compute the center time
    double etCenter = (p_fullCacheEndTime + p_fullCacheStartTime) / 2.;
    SetEphemerisTime(etCenter);

    return Angles(p_axis3, p_axis2, p_axis1);
  }


  /**
   * Return the camera angles (right ascension, declination, and twist) for the
   * time-based matrix CJ
   *
   * @param axis3 The rotation axis for the third angle
   * @param axis2 The rotation axis for the second angle
   * @param axis1 The rotation axis for the first angle
   *
   * @return @b vector<double> Camera angles (ra, dec, twist)
   */
  std::vector<double> Rotation::Angles(int axis3, int axis2, int axis1) {
    NaifStatus::CheckErrors();

    SpiceDouble ang1, ang2, ang3;
    m2eul_c((SpiceDouble *) &p_CJ[0], axis3, axis2, axis1, &ang3, &ang2, &ang1);

    std::vector<double> angles;
    angles.push_back(ang1);
    angles.push_back(ang2);
    angles.push_back(ang3);

    NaifStatus::CheckErrors();
    return angles;
  }


  /**
   * Set the rotation angles (phi, delta, and w) for the current time to define the
   * time-based matrix CJ. This method was created for unitTests and should not
   * be used otherwise.  It only works for cached data with a cache size of 1.
   *
   * @param[in]  angles The angles defining the rotation (phi, delta, and w) in radians
   * @param[in]  axis3    The rotation axis for the third angle
   * @param[in]  axis2    The rotation axis for the second angle
   * @param[in]  axis1    The rotation axis for the first angle
   */
  void Rotation::SetAngles(std::vector<double> angles, int axis3, int axis2, int axis1) {
    eul2m_c(angles[2], angles[1], angles[0], axis3, axis2, axis1, (SpiceDouble (*)[3]) &(p_CJ[0]));
    p_cache[0] = p_CJ;
    // Reset to get the new values
    p_et = -DBL_MAX;
    SetEphemerisTime(p_et);
  }


  /**
   * Accessor method to get the angular velocity
   *
   * @return @b vector<double> Angular velocity
   */
  std::vector<double> Rotation::AngularVelocity() {
    return p_av;
  }


  /**
   * Accessor method to get the frame chain for the constant part of the
   * rotation (ends in target)
   *
   * @return @b vector<int> The frame chain for the constant part of the rotation.
   */
  std::vector<int> Rotation::ConstantFrameChain() {
    return p_constantFrames;
  }


  /**
   * Accessor method to get the frame chain for the rotation (begins in J2000).
   *
   * @return @b vector<int> The frame chain for the rotation.
   */
  std::vector<int> Rotation::TimeFrameChain() {
    return p_timeFrames;
  }


  /**
   * Checks whether the rotation has angular velocities.
   *
   * @return @b bool Indicates whether the rotation has angular velocities.
   */
  bool Rotation::HasAngularVelocity() {
    return p_hasAngularVelocity;
  }


  /**
   * Given a direction vector in the reference frame, return a J2000 direction.
   *
   * @param[in] rVec A direction vector in the reference frame
   *
   * @return vector<double>  A direction vector in J2000 frame.
   */
  std::vector<double> Rotation::J2000Vector(const std::vector<double> &rVec) {
    NaifStatus::CheckErrors();

    std::vector<double> jVec;
    if (rVec.size() == 3) {
      double TJ[3][3];
      mxm_c((SpiceDouble *) &p_TC[0], (SpiceDouble *) &p_CJ[0], TJ);
      jVec.resize(3);
      mtxv_c(TJ, (SpiceDouble *) &rVec[0], (SpiceDouble *) &jVec[0]);
    }

    else if (rVec.size() == 6) {
      // See Naif routine frmchg for the format of the state matrix.  The constant rotation, TC,
      // has a derivative with respect to time of I.
      if (!p_hasAngularVelocity) {
        // throw an error
      }
      std::vector<double> stateTJ(36);
      stateTJ = StateTJ();

      // Now invert (inverse of a state matrix is NOT simply the transpose)
      xpose6_c(&stateTJ[0], (SpiceDouble( *) [6]) &stateTJ[0]);
      double stateJT[6][6];
      invstm_((doublereal *) &stateTJ[0], (doublereal *) stateJT);
      xpose6_c(stateJT, stateJT);
      jVec.resize(6);

      mxvg_c(stateJT, (SpiceDouble *) &rVec[0], 6, 6, (SpiceDouble *) &jVec[0]);
    }
    NaifStatus::CheckErrors();
    return (jVec);
  }


  /**
   * Return the coefficients used to calculate the target body pole ra
   *
   * Return the coefficients used to calculate the target body right
   * ascension without nutation/precession.  The model is a standard
   * quadratic polynomial in time in Julian centuries (36525 days) from
   * the standard epoch (usually J2000).  To match the Naif PCK pole
   * right ascension (usually the same as the most recent IAU Report)
   * the trignometric terms to account for nutation/precession need to
   * be added.
   *
   * pole ra = ra0 + ra1*T + ra2*T**2 + sum(racoef[i]i*sin(angle[i]))
   *
   * @return @b vector<double> A vector of length 3 containing the pole ra coefficients
   */
  std::vector<Angle> Rotation::poleRaCoefs() {
    return m_raPole;
  }


  /**
   * Return the coefficients used to calculate the target body pole dec
   *
   * Return the coefficients used to calculate the target body declination
   * without nutation/precession.  The model is a standard quadratic
   * polynomial in time in Julian centuries (36525 days) from
   * the standard epoch (usually J2000).  To match the Naif PCK pole
   * declination (usually the same as the most recent IAU Report)
   * the trignometric terms to account for nutation/precession need to
   * be added.
   *
   * pole dec = dec0 + dec1*T + dec2*T**2 + sum(racoef[i]i*sin(angle[i]))
   *
   * @return @b vector<double> A vector of length 3 containing the pole declination coefficients.
   */
  std::vector<Angle> Rotation::poleDecCoefs() {
    return m_decPole;
  }


  /**
   * Return the coefficients used to calculate the target body prime meridian
   *
   * Return the coefficients used to calculate the target body prime
   * meridian without nutation/precession.  The model is a standard
   * quadratic polynomial in time in days from the standard epoch
   * (usually J2000).  To match the Naif PCK prime meridian, (usually
   * the same as the most recent IAU Report) the trignometric terms
   * to account for nutation/precession need to be added.
   *
   * pm = pm0 + pm1*d + pm2*d**2 + sum(pmcoef[i]i*sin(angle[i]))
   *
   * @return @b vector<double> A vector of length 3 containing the prime meridian coefficients.
   */
  std::vector<Angle> Rotation::pmCoefs() {
    return m_pm;
  }


  /**
   * Return the coefficients used to calculate the target body pole ra nut/prec coefficients
   *
   * Return the coefficients used to calculate the target body right
   * ascension nutation/precession contribution.  The model is the
   * sum of the products of the coefficients returned by this method
   * with the sin of the corresponding angles associated with the
   * barycenter related to the target body.
   *
   * pole ra = ra0 + ra1*T + ra2*T**2 + sum(raCoef[i]i*sin(angle[i]))
   *
   * @return @b vector<double> A vector containing the pole ra nut/prec coefficients.
   */
  std::vector<double> Rotation::poleRaNutPrecCoefs() {
    return m_raNutPrec;
  }


  /**
   * Return the coefficients used to calculate the target body pole dec nut/prec coefficients
   *
   * Return the coefficients used to calculate the target body declination
   * nutation/precession contribution.  The model is the sum of the products
   * of the coefficients returned by this method with the sin of the corresponding
   * angles associated with the barycenter related to the target body.
   *
   * pole dec = dec0 + dec1*T + dec2*T**2 + sum(decCoef[i]i*sin(angle[i]))
   *
   * @return @b vector<double> A vector containing the pole dec nut/prec coeffcients.
   */
  std::vector<double> Rotation::poleDecNutPrecCoefs() {
    return m_decNutPrec;
  }


  /**
   * Return the coefficients used to calculate the target body pm nut/prec coefficients
   *
   * Return the coefficients used to calculate the target body prime meridian
   * nutation/precession contribution.  The model is the sum of the products
   * of the coefficients returned by this method with the sin of the corresponding
   * angles associated with the barycenter related to the target body.
   *
   * prime meridian = pm0 + pm1*T + pm2*d**2 + sum(pmCoef[i]i*sin(angle[i]))
   *
   * @return @b vector<double> A vector containing the pm nut/prec coeffcients.
   */
  std::vector<double> Rotation::pmNutPrecCoefs() {
    return m_pmNutPrec;
  }


  /**
   * Return the constants used to calculate the target body system nut/prec angles
   *
   * Return the constant terms used to calculate the target body system (barycenter)
   * nutation/precession angles (periods).  The model for the angles is linear in
   * time in Julian centuries since the standard epoch (usually J2000).
   * angles associated with the barycenter related to the target body.
   *
   * angle[i] = constant[i] + coef[i]*T
   *
   * @return @b vector<Angle> A vector containing the system nut/prec constant terms.
   */
  std::vector<Angle> Rotation::sysNutPrecConstants() {
    return m_sysNutPrec0;
  }


  /**
   * Return the coefficients used to calculate the target body system nut/prec angles
   *
   * Return the linear coefficients used to calculate the target body system
   * (barycenter) nutation/precession angles (periods).  The model for the
   * angles is linear in time in Julian centuries since the standard epoch
   * (usually J2000). angles associated with the barycenter related to the
   * target body.
   *
   * angle[i] = constant[i] + coef[i]*T
   *
   * @return @b vector<Angle> A vector containing the system nut/prec linear coefficients.
   */
  std::vector<Angle> Rotation::sysNutPrecCoefs() {
    return m_sysNutPrec1;
  }


  /**
   * Given a direction vector in the reference frame, compute the derivative
   * with respect to one of the coefficients in the angle polynomial fit
   * equation of a vector rotated from the reference frame to J2000.
   * TODO - merge this method with ToReferencePartial
   *
   * @param[in]  lookT A direction vector in the targeted reference frame
   * @param[in]  partialVar  Variable derivative is to be with respect to
   * @param[in]  coeffIndex  Coefficient index in the polynomial fit to the variable (angle)
   *
   * @throws IException::User "Body rotation uses a binary PCK. Solutions for this model are not
   *                           supported"
   * @throws IException::User "Body rotation uses a PCK not referenced to J2000. Solutions for this
   *                           model are not supported"
   * @throws IException::User "Solutions are not supported for this frame type."
   *
   * @return @b vector<double>   A direction vector rotated by derivative
   *                             of reference to J2000 rotation.
   */
  std::vector<double> Rotation::toJ2000Partial(const std::vector<double> &lookT,
                                                    Rotation::PartialType partialVar,
                                                    int coeffIndex) {
    NaifStatus::CheckErrors();

    std::vector<double> jVec;

    // Get the rotation angles and form the derivative matrix for the partialVar
    std::vector<double> angles = Angles(p_axis3, p_axis2, p_axis1);
    int angleIndex = partialVar;
    int axes[3] = {p_axis1, p_axis2, p_axis3};

    double angle = angles.at(angleIndex);

    // Get TJ and apply the transpose to the input vector to get it to J2000
    double dmatrix[3][3];
    drotat_(&angle, (integer *) axes + angleIndex, (doublereal *) dmatrix);
    // Transpose to obtain row-major order
    xpose_c(dmatrix, dmatrix);

    // Get the derivative of the polynomial with respect to partialVar
    double dpoly = 0.;
    QString msg;
    switch (m_frameType) {
     case CK:
     case DYN:
       dpoly = DPolynomial(coeffIndex);
       break;
     case PCK:
       dpoly = DPckPolynomial(partialVar, coeffIndex);
       break;
     case BPC:
       msg = "Body rotation uses a binary PCK.  Solutions for this model are not supported";
       throw IException(IException::User, msg, _FILEINFO_);
       break;
     case NOTJ2000PCK:
       msg = "Body rotation uses a PCK not referenced to J2000. "
             "Solutions for this model are not supported";
       throw IException(IException::User, msg, _FILEINFO_);
       break;
     default:
       msg = "Solutions are not supported for this frame type.";
       throw IException(IException::User, msg, _FILEINFO_);
    }

    // Multiply dpoly to complete dmatrix
    for (int row = 0;  row < 3;  row++) {
      for (int col = 0;  col < 3;  col++) {
        dmatrix[row][col] *= dpoly;
      }
    }

    // Apply the other 2 angles and chain them all together to get J2000 to constant frame
    double dCJ[3][3];
    switch (angleIndex) {
      case 0:
        rotmat_c(dmatrix, angles[1], axes[1], dCJ);
        rotmat_c(dCJ, angles[2], axes[2], dCJ);
        break;
      case 1:
        rotate_c(angles[0], axes[0], dCJ);
        mxm_c(dmatrix, dCJ, dCJ);
        rotmat_c(dCJ, angles[2], axes[2], dCJ);
        break;
      case 2:
        rotate_c(angles[0], axes[0], dCJ);
        rotmat_c(dCJ, angles[1], axes[1], dCJ);
        mxm_c(dmatrix, dCJ, dCJ);
        break;
    }

    // Multiply the constant matrix to rotate to target reference frame
    double dTJ[3][3];
    mxm_c((SpiceDouble *) &p_TC[0], dCJ[0], dTJ);

    // Finally rotate the target vector with the transpose of the
    // derivative matrix, dTJ to get a J2000 vector
    std::vector<double> lookdJ(3);

    mtxv_c(dTJ, (const SpiceDouble *) &lookT[0], (SpiceDouble *) &lookdJ[0]);

    NaifStatus::CheckErrors();
    return (lookdJ);
  }


  /**
   * Accessor method to get current ephemeris time.
   *
   * @return @b double The current ephemeris time.
   */
  double Rotation::EphemerisTime() const {
    return p_et;
  }


  /**
   * Checks if the cache is empty.
   *
   * @return @b bool Indicates whether this rotation is cached.
   */
  bool Rotation::IsCached() const {
    return (p_cache.size() > 0);
  }
  /**
   * Given a direction vector in J2000, return a reference frame direction.
   *
   * @param[in] jVec A direction vector in J2000
   *
   * @return @b vector<double> A direction vector in reference frame.
   */
  std::vector<double> Rotation::ReferenceVector(const std::vector<double> &jVec) {
    NaifStatus::CheckErrors();

    std::vector<double> rVec(3);

    if (jVec.size() == 3) {
      double TJ[3][3];
      mxm_c((SpiceDouble *) &p_TC[0], (SpiceDouble *) &p_CJ[0], TJ);
      rVec.resize(3);
      mxv_c(TJ, (SpiceDouble *) &jVec[0], (SpiceDouble *) &rVec[0]);
    }
    else if (jVec.size() == 6) {
      // See Naif routine frmchg for the format of the state matrix.  The constant rotation, TC,
      // has a derivative with respect to time of I.
      if (!p_hasAngularVelocity) {
        // throw an error
      }
      std::vector<double>  stateTJ(36);
      stateTJ = StateTJ();
      rVec.resize(6);
      mxvg_c((SpiceDouble *) &stateTJ[0], (SpiceDouble *) &jVec[0], 6, 6, (SpiceDouble *) &rVec[0]);
    }

    NaifStatus::CheckErrors();
    return (rVec);
  }
i
 /**
   * Set an override base time to be used with observations on scanners to allow all
   * images in an observation to use the save base time and polynomials for the angles.
   *
   * @param[in] baseTime The base time to use and override the computed base time
   * @param[in] timeScale The time scale to use and override the computed time scale
   */
  void Rotation::SetOverrideBaseTime(double baseTime, double timeScale) {
    p_overrideBaseTime = baseTime;
    p_overrideTimeScale = timeScale;
    p_noOverride = false;
    return;
 }

  void Rotation::SetCacheTime(std::vector<double> cacheTime) {
    // Do not reset the cache times if they are already loaded.
    if (p_cacheTime.size() <= 0) {
      p_cacheTime = cacheTime;
    }
  }

  /**
   * Wrap the input angle to keep it within 2pi radians of the angle to compare.
   *
   * @param[in]  compareAngle Look vector in J2000 frame
   * @param[in]  angle Angle to be wrapped if needed
   *
   * @return @b double Wrapped angle.
   */
  double Rotation::WrapAngle(double compareAngle, double angle) {
    NaifStatus::CheckErrors();
    double diff1 = compareAngle - angle;

    if (diff1 < -1 * pi_c()) {
      angle -= twopi_c();
    }
    else if (diff1 > pi_c()) {
      angle += twopi_c();
    }

    NaifStatus::CheckErrors();
    return angle;
  }

  /**
   * Accessor method to get the rotation frame type.
   *
   * @return @b Rotation::FrameType The frame type of the rotation.
   */
  Rotation::FrameType Rotation::getFrameType() {
    return m_frameType;
  }


  /**
   * Change the frame to the given frame code.  This method has no effect if
   * spice is cached.
   *
   * @param frameCode The integer-valued frame code
   */
  void RotationMemcache::SetFrame(int frameCode) {
    p_constantFrames[0] = frameCode;
  }


  /**
   * Accessor method that returns the frame code. This is the first value of the
   * constant frames member variable.
   *
   * @return @b int An integer value indicating the frame code.
   */
  int RotationMemcache::Frame() {
    return p_constantFrames[0];
  }


  /**
   * Apply a time bias when invoking SetEphemerisTime method.
   *
   * The bias is used only when reading from NAIF kernels.  It is added to the
   * ephermeris time passed into SetEphemerisTime and then the body
   * position is read from the NAIF kernels and returned.  When the cache
   * is loaded from a table the bias is ignored as it is assumed to have
   * already been applied.  If this method is never called the default bias is
   * 0.0 seconds.
   *
   * @param timeBias time bias in seconds
   */
  void RotationMemcache::SetTimeBias(double timeBias) {
    p_timeBias = timeBias;
  }


  /**
   * Accessor method to get the rotation base time.
   *
   * @return @b double The base time for the rotation.
   */
  double Rotation::GetBaseTime() {
    return p_baseTime;
  }


  /**
   * Accessor method to get the rotation time scale.
   *
   * @return @b double The time scale for the rotation.
   */
  double Rotation::GetTimeScale() {
    return p_timeScale;
  }


  /**
   * Set the axes of rotation for decomposition of a rotation
   * matrix into 3 angles.
   *
   * @param[in]  axis1 Axes of rotation of first angle applied (right rotation)
   * @param[in]  axis2 Axes of rotation of second angle applied (center rotation)
   * @param[in]  axis3 Axes of rotation of third angle applied (left rotation)
   *
   * @throws IException::Programmer "A rotation axis is outside the valid range of 1 to 3"
   *
   * @return @b double Wrapped angle.
   */
  void Rotation::SetAxes(int axis1, int axis2, int axis3) {
    if (axis1 < 1  ||  axis2 < 1  || axis3 < 1  || axis1 > 3  || axis2 > 3  || axis3 > 3) {
      QString msg = "A rotation axis is outside the valid range of 1 to 3";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    p_axis1 = axis1;
    p_axis2 = axis2;
    p_axis3 = axis3;
  } 


  /**
   * Return full listing (cache) of original time coverage requested.
   *
   * @throws IException::User "Time cache not availabe -- rerun spiceinit"
   *
   * @return @b vector<double> Cache of original time coverage.
   */
  std::vector<double> Rotation::GetFullCacheTime() {

    // No time cache was initialized -- throw an error
    if (p_fullCacheSize < 1) {
      QString msg = "Time cache not available -- rerun spiceinit";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    std::vector<double> fullCacheTime;
    double cacheSlope = 0.0;
    if (p_fullCacheSize > 1) {
      cacheSlope = (p_fullCacheEndTime - p_fullCacheStartTime) / (double)(p_fullCacheSize - 1);
    }

    for (int i = 0; i < p_fullCacheSize; i++)
      fullCacheTime.push_back(p_fullCacheStartTime + (double) i * cacheSlope);

    return fullCacheTime;
  }



  /**
   * Return the full rotation TJ as a matrix
   *
   * @return @b vector<double> Returned matrix.
   */
  std::vector<double> Rotation::Matrix() {
    NaifStatus::CheckErrors();
    std::vector<double> TJ;
    TJ.resize(9);
    mxm_c((SpiceDouble *) &p_TC[0], (SpiceDouble *) &p_CJ[0], (SpiceDouble( *) [3]) &TJ[0]);
    NaifStatus::CheckErrors();
    return TJ;
  }


  /**
   * Return the constant 3x3 rotation TC matrix as a quaternion.
   *
   * @return @b vector<double> Constant rotation quaternion, TC.
   */
  std::vector<double> Rotation::ConstantRotation() {
    NaifStatus::CheckErrors();
    std::vector<double> q;
    q.resize(4);
    m2q_c((SpiceDouble( *)[3]) &p_TC[0], &q[0]);
    NaifStatus::CheckErrors();
    return q;
  }


  /**
   * Return the constant 3x3 rotation TC matrix as a vector of length 9.
   *
   * @return @b vector<double> Constant rotation matrix, TC.
   */
  std::vector<double> &Rotation::ConstantMatrix() {
    return p_TC;
  }


  /**
   * Set the constant 3x3 rotation TC matrix from a vector of length 9.
   *
   * @param constantMatrix Constant rotation matrix, TC.
   */
  void Rotation::SetConstantMatrix(std::vector<double> constantMatrix) {
    p_TC = constantMatrix;
    return;
  }

  /**
   * Return time-based 3x3 rotation CJ matrix as a quaternion.
   *
   * @return @b vector<double> Time-based rotation quaternion, CJ.
   */
  std::vector<double> Rotation::TimeBasedRotation() {
    NaifStatus::CheckErrors();
    std::vector<double> q;
    q.resize(4);
    m2q_c((SpiceDouble( *)[3]) &p_CJ[0], &q[0]);
    NaifStatus::CheckErrors();
    return q;
  }


  /**
   * Return time-based 3x3 rotation CJ matrix as a vector of length 9.
   *
   * @return @b vector<double> Time-based rotation matrix, CJ.
   */
  std::vector<double> &Rotation::TimeBasedMatrix() {
    return p_CJ;
  }


  /**
   * Set the time-based 3x3 rotation CJ matrix from a vector of length 9.
   *
   * @param timeBasedMatrix Time-based rotation matrix, TC.
   */
  void Rotation::SetTimeBasedMatrix(std::vector<double> timeBasedMatrix) {
    p_CJ = timeBasedMatrix;
    return;
  }


  /**
   * Compute the derivative of the 3x3 rotation matrix CJ with respect to time.
   * The derivative is computed based on p_CJ (J2000 to first constant frame).
   *
   * @param[out]  dCJ       Derivative of p_CJ
   */
  void Rotation::DCJdt(std::vector<double> &dCJ) {
    NaifStatus::CheckErrors();

    // Get the rotation angles and axes
    std::vector<double> angles = Angles(p_axis3, p_axis2, p_axis1);
    int axes[3] = {p_axis1, p_axis2, p_axis3};

    double dmatrix[3][3];
    double dangle;
    double wmatrix[3][3]; // work matrix
    dCJ.assign(9, 0.);

    for (int angleIndex = 0; angleIndex < 3; angleIndex++) {
      drotat_(&(angles[angleIndex]), (integer *) axes + angleIndex, (doublereal *) dmatrix);
      // Transpose to obtain row-major order
      xpose_c(dmatrix, dmatrix);

      // To get the derivative of the polynomial fit to the angle with respect to time
      // first create the function object for this angle and load its coefficients
      Isis::PolynomialUnivariate function(p_degree);
      function.SetCoefficients(p_coefficients[angleIndex]);

      // Evaluate the derivative of function at p_et
      //      dangle = function.DerivativeVar((p_et - p_baseTime) / p_timeScale);
      dangle = function.DerivativeVar((p_et - p_baseTime) / p_timeScale) / p_timeScale;

      // Multiply dangle to complete dmatrix
      for (int row = 0;  row < 3;  row++) {
        for (int col = 0;  col < 3;  col++) {
          dmatrix[row][col] *= dangle;
        }
      }
      // Apply the other 2 angles and chain them all together
      switch (angleIndex) {
        case 0:
          rotmat_c(dmatrix, angles[1], axes[1], dmatrix);
          rotmat_c(dmatrix, angles[2], axes[2], dmatrix);
          break;
        case 1:
          rotate_c(angles[0], axes[0], wmatrix);
          mxm_c(dmatrix, wmatrix, dmatrix);
          rotmat_c(dmatrix, angles[2], axes[2], dmatrix);
          break;
        case 2:
          rotate_c(angles[0], axes[0], wmatrix);
          rotmat_c(wmatrix, angles[1], axes[1], wmatrix);
          mxm_c(dmatrix, wmatrix, dmatrix);
          break;
      }
      int i, j;
      for (int index = 0; index < 9; index++) {
        i = index / 3;
        j = index % 3;
        dCJ[index] += dmatrix[i][j];
      }
    }

    NaifStatus::CheckErrors();
  }


  /**
   * Compute & return the rotation matrix that rotates vectors from J2000 to the targeted frame.
   *
   * @return @b vector<double> Returned rotation matrix.
   */
  std::vector<double> Rotation::StateTJ() {
    std::vector<double> stateTJ(36);

    // Build the state matrix for the time-based rotation from the matrix and angulary velocity
    double stateCJ[6][6];
    rav2xf_c(&p_CJ[0], &p_av[0], stateCJ);
// (SpiceDouble (*) [3]) &p_CJ[0]
    int irow = 0;
    int jcol = 0;
    int vpos = 0;

    for (int row = 3; row < 6; row++) {
      irow  =  row - 3;
      vpos  =  irow * 3;

      for (int col = 0; col < 3; col++) {
        jcol  =  col + 3;
        // Fill the upper left corner
        stateTJ[irow*6 + col] = p_TC[vpos] * stateCJ[0][col] + p_TC[vpos+1] * stateCJ[1][col]
                                              + p_TC[vpos+2] * stateCJ[2][col];
        // Fill the lower left corner
        stateTJ[row*6 + col]  =  p_TC[vpos] * stateCJ[3][col] + p_TC[vpos+1] * stateCJ[4][col]
                                              + p_TC[vpos+2] * stateCJ[5][col];
        // Fill the upper right corner
        stateTJ[irow*6 + jcol] = 0;
        // Fill the lower right corner
        stateTJ[row*6 +jcol] = stateTJ[irow*6 + col];
      }
    }
    return stateTJ;
  }


  /**
   * Extrapolate pointing for a given time assuming a constant angular velocity.
   * The pointing and angular velocity at the current time will be used to
   * extrapolate pointing at the input time.  If angular velocity does not
   * exist, the value at the current time will be output.
   *
   * @param[in]   timeEt    The time of the pointing to be extrapolated
   *
   * @return @b vector<double> A quaternion defining the rotation at the input time.
   */
   std::vector<double> Rotation::Extrapolate(double timeEt) {
    NaifStatus::CheckErrors();

    if (!p_hasAngularVelocity) return p_CJ;

    double diffTime = timeEt - p_et;
    std::vector<double> CJ(9, 0.0);
    double dmat[3][3];

    // Create a rotation matrix for the axis and magnitude of the angular velocity multiplied by
    //   the time difference
    axisar_c((SpiceDouble *) &p_av[0], diffTime*vnorm_c((SpiceDouble *) &p_av[0]), dmat);

    // Rotate from the current time to the desired time assuming constant angular velocity
    mxm_c(dmat, (SpiceDouble *) &p_CJ[0], (SpiceDouble( *)[3]) &CJ[0]);
    NaifStatus::CheckErrors();
    return CJ;
   }


   /**
    * Set the full cache time parameters.
    *
    * @param[in]   startTime The earliest time of the full cache coverage
    * @param[in]   endTime   The latest time of the full cache coverage
    * @param[in]   cacheSize The number of epochs in the full (line) cache
    */
   void Rotation::SetFullCacheParameters(double startTime, double endTime, int cacheSize) {
     // Save full cache parameters
     p_fullCacheStartTime = startTime;
     p_fullCacheEndTime = endTime;
     p_fullCacheSize = cacheSize;
   }
}
