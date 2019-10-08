#include "EphemerisRotation.h"

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
   * Construct an empty EphemerisRotation class using a valid Naif frame code to
   * set up for getting rotation from Spice kernels.  See required reading
   * ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/naif_ids.html
   *
   * @param frameCode Valid naif frame code.
   */
  EphemerisRotation::EphemerisRotation(int frameCode) {
    p_constantFrames.push_back(frameCode);
    p_timeBias = 0.0;
    p_source = Spice;
    p_CJ.resize(9);
    p_matrixSet = false;
    p_et = -DBL_MAX;
    p_degree = 2;
    p_degreeApplied = false;
    p_noOverride = true;
    p_axis1 = 3;
    p_axis2 = 1;
    p_axis3 = 3;
    p_minimizeCache = No;
    p_hasAngularVelocity = false;
    p_av.resize(3);
    p_fullCacheStartTime = 0;
    p_fullCacheEndTime = 0;
    p_fullCacheSize = 0;
    m_frameType = UNKNOWN;
    m_tOrientationAvailable = false;
  }


  /**
   * Construct an empty EphemerisRotation object using valid Naif frame code and.
   * body code to set up for computing nadir rotation.  See required reading
   * ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/naif_ids.html
   *
   * @param frameCode Valid naif frame code.
   * @param targetCode Valid naif body code.
   *
   * @throws IException::Io "Cannot find [key] in text kernels"
   */
  EphemerisRotation::EphemerisRotation(int frameCode, int targetCode) {
    NaifStatus::CheckErrors();

    p_constantFrames.push_back(frameCode);
    p_targetCode = targetCode;
    p_timeBias = 0.0;
    p_source = Nadir;
    p_CJ.resize(9);
    p_matrixSet = false;
    p_et = -DBL_MAX;
    p_axisP = 3;
    p_degree = 2;
    p_degreeApplied = false;
    p_noOverride = true;
    p_axis1 = 3;
    p_axis2 = 1;
    p_axis3 = 3;
    p_minimizeCache = No;
    p_hasAngularVelocity = false;
    p_av.resize(3);
    p_fullCacheStartTime = 0;
    p_fullCacheEndTime = 0;

    p_fullCacheSize = 0;
    m_frameType = DYN;
    m_tOrientationAvailable = false;

    // Determine the axis for the velocity vector
    QString key = "INS" + toString(frameCode) + "_TRANSX";
    SpiceDouble transX[2];
    SpiceInt number;
    SpiceBoolean found;
    //Read starting at element 1 (skipping element 0)
    gdpool_c(key.toLatin1().data(), 1, 2, &number, transX, &found);

    if (!found) {
      QString msg = "Cannot find [" + key + "] in text kernels";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    p_axisV = 2;
    if (transX[0] < transX[1]) p_axisV = 1;

    NaifStatus::CheckErrors();
  }


  /**
   * Construct a EphemerisRotation object by copying from an existing one.
   *
   * @param rotToCopy const reference to other EphemerisRotation to copy
   */
  EphemerisRotation::EphemerisRotation(const EphemerisRotation &rotToCopy) {
    p_cacheTime = rotToCopy.p_cacheTime;
    p_cache = rotToCopy.p_cache;
    p_cacheAv = rotToCopy.p_cacheAv;
    p_av = rotToCopy.p_av;
    p_degree = rotToCopy.p_degree;
    p_axis1 = rotToCopy.p_axis1;
    p_axis2 = rotToCopy.p_axis2;
    p_axis3 = rotToCopy.p_axis3;

    p_constantFrames = rotToCopy.p_constantFrames;
    p_timeFrames = rotToCopy.p_timeFrames;
    p_timeBias = rotToCopy.p_timeBias;

    p_et = rotToCopy.p_et;
    p_quaternion = rotToCopy.p_quaternion;
    p_matrixSet = rotToCopy.p_matrixSet;
    p_source = rotToCopy.p_source;
    p_axisP = rotToCopy.p_axisP;
    p_axisV = rotToCopy.p_axisV;
    p_targetCode = rotToCopy.p_targetCode;
    p_baseTime = rotToCopy.p_baseTime;
    p_timeScale = rotToCopy.p_timeScale;
    p_degreeApplied = rotToCopy.p_degreeApplied;

//    for (std::vector<double>::size_type i = 0; i < rotToCopy.p_coefficients[0].size(); i++)
    for (int i = 0; i < 3; i++)
      p_coefficients[i] = rotToCopy.p_coefficients[i];

    p_noOverride = rotToCopy.p_noOverride;
    p_overrideBaseTime = rotToCopy.p_overrideBaseTime;
    p_overrideTimeScale = rotToCopy.p_overrideTimeScale;
    p_minimizeCache = rotToCopy.p_minimizeCache;
    p_fullCacheStartTime = rotToCopy.p_fullCacheStartTime;
    p_fullCacheEndTime = rotToCopy.p_fullCacheEndTime;
    p_fullCacheSize = rotToCopy.p_fullCacheSize;
    p_TC = rotToCopy.p_TC;

    p_CJ = rotToCopy.p_CJ;
    p_degree = rotToCopy.p_degree;
    p_hasAngularVelocity = rotToCopy.p_hasAngularVelocity;
    m_frameType = rotToCopy.m_frameType;

  }


  /**
   * Destructor for EphemerisRotation object.
   */
  EphemerisRotation::~EphemerisRotation() {
  }


  /**
   * Return the J2000 to reference frame quaternion at given time.
   *
   * This method returns the J2000 to reference frame rotational matrix at a
   * given et in seconds.  The quaternion is obtained from either valid NAIF ck
   * and/or fk, or alternatively from an internal cache loaded from an ISIS
   * Table object.  In the first case, the kernels must contain the rotation
   * for the frame specified in the constructor at the given time (as well as
   * all the intermediate frames going from the reference frame to J2000) and
   * they must be loaded using the SpiceKernel class.
   *
   * @param et   ephemeris time in seconds
   */
  void EphemerisRotation::SetEphemerisTime(double et) {

    // Save the time
    if (p_et == et) return;
    p_et = et;

    // If the cache has only one rotation, set it
    NaifStatus::CheckErrors();
    if (p_cache.size() == 1) {
      p_CJ = p_cache[0];
      if (p_hasAngularVelocity) {
        p_av = p_cacheAv[0];
      }
    }
    // Otherwise determine the interval to interpolate
    else {
      std::vector<double>::iterator pos;
      pos = upper_bound(p_cacheTime.begin(), p_cacheTime.end(), p_et);

      int cacheIndex;

      if (pos != p_cacheTime.end()) {
        cacheIndex = distance(p_cacheTime.begin(), pos);
        cacheIndex--;
      }
      else {
        cacheIndex = p_cacheTime.size() - 2;
      }

      if (cacheIndex < 0) cacheIndex = 0;

      // Interpolate the rotation
      double mult = (p_et - p_cacheTime[cacheIndex]) /
                    (p_cacheTime[cacheIndex+1] - p_cacheTime[cacheIndex]);
      /*        Quaternion Q2 (p_cache[cacheIndex+1]);
               Quaternion Q1 (p_cache[cacheIndex]);*/
      std::vector<double> CJ2(p_cache[cacheIndex+1]);
      std::vector<double> CJ1(p_cache[cacheIndex]);
      SpiceDouble J2J1[3][3];
      mtxm_c((SpiceDouble( *)[3]) &CJ2[0], (SpiceDouble( *)[3]) &CJ1[0], J2J1);
      SpiceDouble axis[3];
      SpiceDouble angle;
      raxisa_c(J2J1, axis, &angle);
      SpiceDouble delta[3][3];
      axisar_c(axis, angle * (SpiceDouble)mult, delta);
      mxmt_c((SpiceDouble *) &CJ1[0], delta, (SpiceDouble( *) [3]) &p_CJ[0]);

      if (p_hasAngularVelocity) {
        double v1[3], v2[3]; // Vectors surrounding desired time
        vequ_c((SpiceDouble *) &p_cacheAv[cacheIndex][0], v1);
        vequ_c((SpiceDouble *) &p_cacheAv[cacheIndex+1][0], v2);
        vscl_c((1. - mult), v1, v1);
        vscl_c(mult, v2, v2);
        vadd_c(v1, v2, (SpiceDouble *) &p_av[0]);
      }
    }
    NaifStatus::CheckErrors();
  }


  /**
   * Set the downsize status to minimize cache.
   *
   * @param status The DownsizeStatus enumeration value.
   */
  void EphemerisRotation::MinimizeCache(DownsizeStatus status) {
    p_minimizeCache = status;
  } 

  void EphemerisRotation::LoadCache(json &isdRot){
    if (p_source != Spice) {
        throw IException(IException::Programmer, "EphemerisRotation::LoadCache(json) only support Spice source", _FILEINFO_);
    }

    p_timeFrames.clear();
    p_TC.clear();
    p_cache.clear();
    p_cacheTime.clear();
    p_cacheAv.clear();
    p_hasAngularVelocity = false;
    m_frameType = PCK;

    // Load the full cache time information from the label if available
    p_fullCacheStartTime = isdRot["CkTableStartTime"].get<double>();
    p_fullCacheEndTime = isdRot["CkTableEndTime"].get<double>();
    p_fullCacheSize = isdRot["CkTableOriginalSize"].get<double>();
    p_cacheTime = isdRot["EphemerisTimes"].get<std::vector<double>>();
    p_timeFrames = isdRot["TimeDependentFrames"].get<std::vector<int>>();

    m_raPole.resize(3);
    m_raPole[0].setDegrees(0);
    m_raPole[0].setDegrees(0);
    m_raPole[0].setDegrees(0);

    m_decPole.resize(3);
    m_decPole[0].setDegrees(0);
    m_decPole[0].setDegrees(0);
    m_decPole[0].setDegrees(0);

    m_pm.resize(3);
    m_pm[0].setDegrees(0);
    m_pm[0].setDegrees(0);
    m_pm[0].setDegrees(0);

    for (auto it = isdRot["Quaternions"].begin(); it != isdRot["Quaternions"].end(); it++) {
        std::vector<double> quat = {it->at(0).get<double>(), it->at(1).get<double>(), it->at(2).get<double>(), it->at(3).get<double>()};
        Quaternion q(quat);
        std::vector<double> CJ = q.ToMatrix();
        p_cache.push_back(CJ);
    }

    bool hasConstantFrames = isdRot.find("ConstantFrames") != isdRot.end();
    bool hasConstantRotation = isdRot.find("ConstantRotation") != isdRot.end();

    if (hasConstantFrames) {
      p_constantFrames = isdRot["ConstantFrames"].get<std::vector<int>>();
      p_TC = isdRot["ConstantRotation"].get<std::vector<double>>();

    }
    else {
      p_TC.resize(9);
      ident_c((SpiceDouble( *)[3]) &p_TC[0]);
    }

    p_source = Memcache;
    SetEphemerisTime(p_cacheTime[0]);
  }


  /**
   * Cache J2000 rotations using a table file.
   *
   * This method will load either an internal cache with rotations (quaternions)
   * or coefficients (for 3 polynomials defining the camera angles) from an
   * ISIS table file.  In the first case, the table must have 5 columns and
   * at least one row. The 5 columns contain the following information, J2000
   * to reference quaternion (q0, q1, q2, q3)  and the ephemeris time of that
   * position. If there are multiple rows, it is assumed the quaternions between
   * the rows can be interpolated.  In the second case, the table must have
   * three columns and at least two rows.  The three columns contain the
   * coefficients for each of the three camera angles.  Row one of the
   * table contains coefficient 0 (constant term) for angles 1, 2, and 3.
   * If the degree of the fit equation is greater than 1, row 2 contains
   * coefficient 1 (linear) for each of the three angles.  Row n contains
   * coefficient n-1 and the last row contains the time parameters, base time,
   * and time scale, and the degree of the polynomial.
   *
   * @param table   An ISIS table blob containing valid J2000 to reference
   *                quaternion/time values
   *
   * @throws IException::Programmer "Expecting either three, five, or eight fields in the
   *                                 EphemerisRotation table"
   */
  void EphemerisRotation::LoadCache(Table &table) {
    // Clear any existing cached data to make it reentrant (KJB 2011-07-20).
    p_timeFrames.clear();
    p_TC.clear();
    p_cache.clear();
    p_cacheTime.clear();
    p_cacheAv.clear();
    p_hasAngularVelocity = false;

    // Load the constant and time-based frame traces and the constant rotation
    if (table.Label().hasKeyword("TimeDependentFrames")) {
      PvlKeyword labelTimeFrames = table.Label()["TimeDependentFrames"];
      for (int i = 0; i < labelTimeFrames.size(); i++) {
        p_timeFrames.push_back(toInt(labelTimeFrames[i]));
      }
    }
    else {
      p_timeFrames.push_back(p_constantFrames[0]);
      p_timeFrames.push_back(J2000Code);
    }

    if (table.Label().hasKeyword("ConstantRotation")) {
      PvlKeyword labelConstantFrames = table.Label()["ConstantFrames"];
      p_constantFrames.clear();

      for (int i = 0; i < labelConstantFrames.size(); i++) {
        p_constantFrames.push_back(toInt(labelConstantFrames[i]));
      }
      PvlKeyword labelConstantRotation = table.Label()["ConstantRotation"];

      for (int i = 0; i < labelConstantRotation.size(); i++) {
        p_TC.push_back(toDouble(labelConstantRotation[i]));
      }
    }
    else {
      p_TC.resize(9);
      ident_c((SpiceDouble( *)[3]) &p_TC[0]);
    }

    // Load the full cache time information from the label if available
    if (table.Label().hasKeyword("CkTableStartTime")) {
      p_fullCacheStartTime = toDouble(table.Label().findKeyword("CkTableStartTime")[0]);
    }
    if (table.Label().hasKeyword("CkTableEndTime")) {
      p_fullCacheEndTime = toDouble(table.Label().findKeyword("CkTableEndTime")[0]);
    }
    if (table.Label().hasKeyword("CkTableOriginalSize")) {
      p_fullCacheSize = toInt(table.Label().findKeyword("CkTableOriginalSize")[0]);
    }

    // Load FrameTypeCode from labels if available and the planetary constants keywords
    if (table.Label().hasKeyword("FrameTypeCode")) {
      m_frameType = (FrameType) toInt(table.Label().findKeyword("FrameTypeCode")[0]);
    }
    else {
      m_frameType = UNKNOWN;
    }

    if (m_frameType  == PCK) {
      loadPCFromTable(table.Label());
    }

    int recFields = table[0].Fields();

    // Loop through and move the table to the cache.  Retrieve the first record to
    // establish the type of cache and then use the appropriate loop.

    // list table of quaternion and time
    if (recFields == 5) {
      for (int r = 0; r < table.Records(); r++) {
        TableRecord &rec = table[r];

        if (rec.Fields() != recFields) {
          // throw and error
        }

        std::vector<double> j2000Quat;
        j2000Quat.push_back((double)rec[0]);
        j2000Quat.push_back((double)rec[1]);
        j2000Quat.push_back((double)rec[2]);
        j2000Quat.push_back((double)rec[3]);

        Quaternion q(j2000Quat);
        std::vector<double> CJ = q.ToMatrix();
        p_cache.push_back(CJ);
        p_cacheTime.push_back((double)rec[4]);
      }
      p_source = Memcache;
    }

    // list table of quaternion, angular velocity vector, and time
    else if (recFields == 8) {
      for (int r = 0; r < table.Records(); r++) {
        TableRecord &rec = table[r];

        if (rec.Fields() != recFields) {
          // throw and error
        }

        std::vector<double> j2000Quat;
        j2000Quat.push_back((double)rec[0]);
        j2000Quat.push_back((double)rec[1]);
        j2000Quat.push_back((double)rec[2]);
        j2000Quat.push_back((double)rec[3]);


        Quaternion q(j2000Quat);
        std::vector<double> CJ = q.ToMatrix();
        p_cache.push_back(CJ);

        std::vector<double> av;
        av.push_back((double)rec[4]);
        av.push_back((double)rec[5]);
        av.push_back((double)rec[6]);
        p_cacheAv.push_back(av);

        p_cacheTime.push_back((double)rec[7]);
        p_hasAngularVelocity = true;
      }
      p_source = Memcache;
    }
    // coefficient table for angle1, angle2, and angle3
    else if (recFields == 3) {
      throw "This is not supported by memcache"; 
    else  {
      QString msg = "Expecting either three, five, or eight fields in the EphemerisRotation table";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }
  }



  /**
   * Initialize planetary orientation constants from an cube body rotation label
   *
   * Retrieve planetary orientation constants from a cube body rotation label if they are present.
   *
   * @param label const reference to the cube body rotation pvl label
   */
  void EphemerisRotation::loadPCFromTable(const PvlObject &label) {
    NaifStatus::CheckErrors();

    // First clear existing cached data
    m_raPole.clear();
    m_decPole.clear();
    m_pm.clear();
    m_raNutPrec.clear();
    m_decNutPrec.clear();
    m_sysNutPrec0.clear();
    m_sysNutPrec1.clear();
    int numLoaded = 0;

    // Load the PCK coeffcients if they are on the label
    if (label.hasKeyword("PoleRa")) {
      PvlKeyword labelCoeffs = label["PoleRa"];
      for (int i = 0; i < labelCoeffs.size(); i++){
        m_raPole.push_back(Angle(toDouble(labelCoeffs[i]), Angle::Degrees));
      }
      numLoaded += 1;
    }
    if (label.hasKeyword("PoleDec")) {
      PvlKeyword labelCoeffs = label["PoleDec"];
      for (int i = 0; i < labelCoeffs.size(); i++){
        m_decPole.push_back(Angle(toDouble(labelCoeffs[i]), Angle::Degrees));
      }
      numLoaded += 1;
    }
    if (label.hasKeyword("PrimeMeridian")) {
      PvlKeyword labelCoeffs = label["PrimeMeridian"];
      for (int i = 0; i < labelCoeffs.size(); i++){
        m_pm.push_back(Angle(toDouble(labelCoeffs[i]), Angle::Degrees));
      }
      numLoaded += 1;
    }
    if (numLoaded > 2) m_tOrientationAvailable = true;

    if (label.hasKeyword("PoleRaNutPrec")) {
      PvlKeyword labelCoeffs = label["PoleRaNutPrec"];
      for (int i = 0; i < labelCoeffs.size(); i++){
        m_raNutPrec.push_back(toDouble(labelCoeffs[i]));
      }
    }
    if (label.hasKeyword("PoleDecNutPrec")) {
      PvlKeyword labelCoeffs = label["PoleDecNutPrec"];
      for (int i = 0; i < labelCoeffs.size(); i++){
        m_decNutPrec.push_back(toDouble(labelCoeffs[i]));
      }
    }
    if (label.hasKeyword("PmNutPrec")) {
      PvlKeyword labelCoeffs = label["PmNutPrec"];
      for (int i = 0; i < labelCoeffs.size(); i++){
        m_pmNutPrec.push_back(toDouble(labelCoeffs[i]));
      }
    }
    if (label.hasKeyword("SysNutPrec0")) {
      PvlKeyword labelCoeffs = label["SysNutPrec0"];
      for (int i = 0; i < labelCoeffs.size(); i++){
        m_sysNutPrec0.push_back(Angle(toDouble(labelCoeffs[i]), Angle::Degrees));
      }
    }
    if (label.hasKeyword("SysNutPrec1")) {
      PvlKeyword labelCoeffs = label["SysNutPrec1"];
      for (int i = 0; i < labelCoeffs.size(); i++){
        m_sysNutPrec1.push_back(Angle(toDouble(labelCoeffs[i]), Angle::Degrees));
      }
    }

    NaifStatus::CheckErrors();
  }


  /**
   * Add labels to a EphemerisRotation table.
   *
   * Return a table containing the labels defining the rotation.
   *
   * @param Table    Table to receive labels
   */
  void EphemerisRotation::CacheLabel(Table &table) {
    NaifStatus::CheckErrors();
    // Load the constant and time-based frame traces and the constant rotation
    // into the table as labels
    if (p_timeFrames.size() > 1) {
      table.Label() += PvlKeyword("TimeDependentFrames");

      for (int i = 0; i < (int) p_timeFrames.size(); i++) {
        table.Label()["TimeDependentFrames"].addValue(toString(p_timeFrames[i]));
      }
    }

    if (p_constantFrames.size() > 1) {
      table.Label() += PvlKeyword("ConstantFrames");

      for (int i = 0; i < (int) p_constantFrames.size(); i++) {
        table.Label()["ConstantFrames"].addValue(toString(p_constantFrames[i]));
      }

      table.Label() += PvlKeyword("ConstantRotation");

      for (int i = 0; i < (int) p_TC.size(); i++) {
        table.Label()["ConstantRotation"].addValue(toString(p_TC[i]));
      }
    }

    // Write original time coverage
    if (p_fullCacheStartTime != 0) {
      table.Label() += PvlKeyword("CkTableStartTime");
      table.Label()["CkTableStartTime"].addValue(toString(p_fullCacheStartTime));
    }
    if (p_fullCacheEndTime != 0) {
      table.Label() += PvlKeyword("CkTableEndTime");
      table.Label()["CkTableEndTime"].addValue(toString(p_fullCacheEndTime));
    }
    if (p_fullCacheSize != 0) {
      table.Label() += PvlKeyword("CkTableOriginalSize");
      table.Label()["CkTableOriginalSize"].addValue(toString(p_fullCacheSize));
    }

 // Begin section added 06-20-2015 DAC
    table.Label() += PvlKeyword("FrameTypeCode");
    table.Label()["FrameTypeCode"].addValue(toString(m_frameType));

    if (m_frameType == PCK) {
      // Write out all the body orientation constants
      // Pole right ascension coefficients for a quadratic equation
      table.Label() += PvlKeyword("PoleRa");

      for (int i = 0; i < (int) m_raPole.size(); i++) {
        table.Label()["PoleRa"].addValue(toString(m_raPole[i].degrees()));
      }

      // Pole right ascension, declination coefficients for a quadratic equation
      table.Label() += PvlKeyword("PoleDec");
      for (int i = 0; i < (int) m_decPole.size(); i++) {
        table.Label()["PoleDec"].addValue(toString(m_decPole[i].degrees()));
      }

      // Prime meridian coefficients for a quadratic equation
      table.Label() += PvlKeyword("PrimeMeridian");
      for (int i = 0; i < (int) m_pm.size(); i++) {
        table.Label()["PrimeMeridian"].addValue(toString(m_pm[i].degrees()));
      }

      if (m_raNutPrec.size() > 0) {
        // Pole right ascension nutation precision coefficients to the trig terms
        table.Label() += PvlKeyword("PoleRaNutPrec");
        for (int i = 0; i < (int) m_raNutPrec.size(); i++) {
          table.Label()["PoleRaNutPrec"].addValue(toString(m_raNutPrec[i]));
        }

        // Pole declination nutation precision coefficients to the trig terms
        table.Label() += PvlKeyword("PoleDecNutPrec");
        for (int i = 0; i < (int) m_decNutPrec.size(); i++) {
          table.Label()["PoleDecNutPrec"].addValue(toString(m_decNutPrec[i]));
        }

        // Prime meridian nutation precision coefficients to the trig terms
        table.Label() += PvlKeyword("PmNutPrec");
        for (int i = 0; i < (int) m_pmNutPrec.size(); i++) {
          table.Label()["PmNutPrec"].addValue(toString(m_pmNutPrec[i]));
        }

        // System nutation precision constant terms of linear model of periods
        table.Label() += PvlKeyword("SysNutPrec0");
        for (int i = 0; i < (int) m_sysNutPrec0.size(); i++) {
          table.Label()["SysNutPrec0"].addValue(toString(m_sysNutPrec0[i].degrees()));
        }

        // System nutation precision linear terms of linear model of periods
        table.Label() += PvlKeyword("SysNutPrec1");
        for (int i = 0; i < (int) m_sysNutPrec1.size(); i++) {
          table.Label()["SysNutPrec1"].addValue(toString(m_sysNutPrec1[i].degrees()));
        }
      }
    }
 // End section added 06-20-2015 DAC

    NaifStatus::CheckErrors();
  }


}
