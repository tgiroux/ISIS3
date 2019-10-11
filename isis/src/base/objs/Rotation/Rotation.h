#ifndef Rotation_h
#define Rotation_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2010/03/27 07:04:26 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <string>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "Angle.h"
#include "Table.h"
#include "PolynomialUnivariate.h"
#include "Quaternion.h"

#define J2000Code    1

namespace Isis {
  class Rotation {
    public:
      // Constructors
      Rotation();
      Rotation(int frameCode);
      /*      Rotation( int NaifCode );
      We would like to call refchg instead to avoid the strings.  Currently Naif does
      not have refchg_c, but only the f2c'd refchg.c.*/
      Rotation(int frameCode, int targetCode);
      Rotation(const Rotation &rotToCopy);

      // Destructor
      virtual ~Rotation();

      // Change the frame (has no effect if cached)
      virtual void SetFrame(int frameCode);
      virtual int Frame();

      virtual void SetTimeBias(double timeBias);

      enum Source {
        Spice,                   //!< Directly from the kernels
        Nadir,                   //!< Nadir pointing
        Memcache,                //!< From cached table
        PolyFunction,            //!< From nth degree polynomial
        PolyFunctionOverSpice ,  //!< Kernels plus nth degree polynomial
        PckPolyFunction          //!< Quadratic polynomial function with linear trignometric terms
      };

      /**
       * This enumeration indicates whether the partial derivative is taken with
       * respect to Right Ascension, Declination, or Twist (or Rotation).
       */
      enum PartialType {
        WRT_RightAscension, //!< With respect to Right Ascension
        WRT_Declination,    //!< With respect to Declination
        WRT_Twist           //!< With respect to Twist or Prime Meridian Rotation
      };

      /**
       * Status of downsizing the cache
       */
      enum DownsizeStatus {
        Yes,  //!< Downsize the cache
        Done, //!< Cache is downsized
        No    //!< Do not downsize the cache
      };

      /**
       * Enumeration for the frame type of the rotation
       */
      enum FrameType {
        UNKNOWN = 0,      //!< Isis specific code for unknown frame type
        INERTL = 1,       //!< See Naif Frames.req document for
        PCK  = 2,         //!< definitions
        CK = 3,           //!<
        TK = 4,           //!<
        DYN = 5,          //!<
        BPC = 6,          //!< Isis specific code for binary pck
        NOTJ2000PCK = 7   //!< PCK frame not referenced to J2000
      };

      virtual void SetEphemerisTime(double et);
      virtual double EphemerisTime() const;

      virtual std::vector<double> GetCenterAngles();

      virtual std::vector<double> Matrix();
      virtual std::vector<double> AngularVelocity();

      // TC
      virtual std::vector<double> ConstantRotation();
      virtual std::vector<double> &ConstantMatrix();
      virtual void SetConstantMatrix(std::vector<double> constantMatrix);

      // CJ
      virtual std::vector<double> TimeBasedRotation();
      virtual std::vector<double> &TimeBasedMatrix();
      virtual void SetTimeBasedMatrix(std::vector<double> timeBasedMatrix);

      virtual std::vector<double> J2000Vector(const std::vector<double> &rVec);

      virtual std::vector<Angle> poleRaCoefs();

      virtual std::vector<Angle> poleDecCoefs();

      virtual std::vector<Angle> pmCoefs();

      virtual std::vector<double> poleRaNutPrecCoefs();

      virtual std::vector<double> poleDecNutPrecCoefs();

      virtual std::vector<double> pmNutPrecCoefs();

      virtual std::vector<Angle> sysNutPrecConstants();

      virtual std::vector<Angle> sysNutPrecCoefs();

      virtual std::vector<double> ReferenceVector(const std::vector<double> &jVec);

      virtual std::vector<double> EvaluatePolyFunction();

      virtual void loadPCFromSpice(int CenterBodyCode);
      virtual void loadPCFromTable(const PvlObject &Label);

      virtual void MinimizeCache(DownsizeStatus status);

      virtual void LoadCache(double startTime, double endTime, int size);

      virtual void LoadCache(double time);

      virtual void LoadCache(Table &table);

      virtual void LoadCache(json &isd);

      virtual Table LineCache(const QString &tableName);

      virtual void ReloadCache();

      virtual Table Cache(const QString &tableName);
      virtual void CacheLabel(Table &table);

      virtual void LoadTimeCache();

      virtual std::vector<double> Angles(int axis3, int axis2, int axis1);
      virtual void SetAngles(std::vector<double> angles, int axis3, int axis2, int axis1);

      virtual bool IsCached() const;

      virtual void SetPolynomial(const Source type=PolyFunction);

      virtual void SetPolynomial(const std::vector<double> &abcAng1,
                         const std::vector<double> &abcAng2,
                         const std::vector<double> &abcAng3,
                         const Source type = PolyFunction);

      virtual void usePckPolynomial();
      virtual void setPckPolynomial(const std::vector<Angle> &raCoeff,
                            const std::vector<Angle> &decCoeff,
                            const std::vector<Angle> &pmCoeff);

      virtual void GetPolynomial(std::vector<double> &abcAng1,
                         std::vector<double> &abcAng2,
                         std::vector<double> &abcAng3);

      virtual void getPckPolynomial(std::vector<Angle> &raCoeff,
                            std::vector<Angle> &decCoeff,
                            std::vector<Angle> &pmCoeff);

      // Set the polynomial degree
      virtual void SetPolynomialDegree(int degree);
      virtual Source GetSource();
      virtual void SetSource(Source source);
      virtual void ComputeBaseTime();
      virtual FrameType getFrameType();
      virtual double GetBaseTime();
      virtual double GetTimeScale();

      virtual void SetOverrideBaseTime(double baseTime, double timeScale);
      virtual void SetCacheTime(std::vector<double> cacheTime); 

      // Derivative methods
      virtual double DPolynomial(const int coeffIndex);
      virtual double DPckPolynomial(PartialType partialVar, const int coeffIndex);

      virtual std::vector<double> toJ2000Partial(const std::vector<double> &lookT,
                                         PartialType partialVar, int coeffIndex);
      virtual std::vector<double> ToReferencePartial(std::vector<double> &lookJ,
                                             PartialType partialVar, int coeffIndex);
      virtual void DCJdt(std::vector<double> &dRJ);

      virtual double WrapAngle(double compareAngle, double angle);
      virtual void SetAxes(int axis1, int axis2, int axis3);
      virtual std::vector<double> GetFullCacheTime();
      virtual void FrameTrace(double et);

      // Return the frame chain for the constant part of the rotation (ends in target)
      virtual std::vector<int>  ConstantFrameChain();
      virtual std::vector<int>  TimeFrameChain();
      virtual void InitConstantRotation(double et);
      virtual bool HasAngularVelocity();

      virtual void ComputeAv();
      virtual std::vector<double> Extrapolate(double timeEt);

      virtual void checkForBinaryPck();
      virtual void setSource(Source source) {
        p_source = source ; 
      }

      int p_degree;                     //!< Degree of fit polynomial for angles
      int p_axis1;                      //!< Axis of rotation for angle 1 of rotation
      int p_axis2;                      //!< Axis of rotation for angle 2 of rotation
      int p_axis3;                      //!< Axis of rotation for angle 3 of rotation
      
      std::vector<int> p_constantFrames;  /**< Chain of Naif frame codes in constant
                                               rotation TC. The first entry will always
                                               be the target frame code*/
      double p_timeBias;                  //!< iTime bias when reading kernels
      Source p_source;                    //!< The source of the rotation data


      // method
      virtual void setFrameType();
      std::vector<int> p_timeFrames;      /**< Chain of Naif frame codes in time-based
                                               rotation CJ. The last entry will always
                                               be 1 (J2000 code)*/
      double p_et;                           //!< Current ephemeris time
      Quaternion p_quaternion;            /**< Quaternion for J2000 to reference
                                                                  rotation at et*/

      bool p_matrixSet;                    //!< Flag indicating p_TJ has been set
      bool m_tOrientationAvailable;  //!< Target orientation constants are available


      FrameType m_frameType;  //!< The type of rotation frame
      int p_axisP;                        /**< The axis defined by the spacecraft
                                               vector for defining a nadir rotation*/
      int p_axisV;                        /**< The axis defined by the velocity
                                               vector for defining a nadir rotation*/
      int p_targetCode;                   //!< For computing Nadir rotation only

      double p_baseTime;                  //!< Base time used in fit equations
      double p_timeScale;                 //!< Time scale used in fit equations
      bool p_degreeApplied;               /**< Flag indicating whether or not a polynomial
                                               of degree p_degree has been created and
                                               used to fill the cache*/
      std::vector<double> p_coefficients[3];  /**< Coefficients defining functions fit
                                                   to 3 pointing angles*/
      bool p_noOverride;                  //!< Flag to compute base time;
      double p_overrideBaseTime;          //!< Value set by caller to override computed base time
      double p_overrideTimeScale;         //!< Value set by caller to override computed time scale
      DownsizeStatus p_minimizeCache;     //!< Status of downsizing the cache (set to No to ignore)
      double p_fullCacheStartTime;        //!< Initial requested starting time of cache
      double p_fullCacheEndTime;          //!< Initial requested ending time of cache
      int p_fullCacheSize;                //!< Initial requested cache size
      std::vector<double> p_TC;           /**< Rotation matrix from first constant rotation
                                          (after all time-based rotations in frame chain from
                                           J2000 to target) to the target frame*/
      std::vector<double> p_CJ;           /**< Rotation matrix from J2000 to first constant
                                               rotation*/
      std::vector<std::vector<double> > p_cacheAv;
      //!< Cached angular velocities for corresponding rotactions in p_cache
      std::vector<double> p_av;           //!< Angular velocity for rotation at time p_et
      bool p_hasAngularVelocity;          /**< Flag indicating whether the rotation
                                               includes angular velocity*/
      virtual std::vector<double> StateTJ();      /**< State matrix (6x6) for rotating state
                                               vectors from J2000 to target frame*/
      // The remaining items are only used for PCK frame types.  In this case the
      // rotation is  stored as a cache, but the coefficients are available for display
      // or comparison, and the first three coefficient sets can be solved for and
      // updated in jigsaw.   The initial coefficient values are read from a Naif PCK.
      //
      // The general equation for the right ascension of the pole is
      //
      // raPole  =  raPole[0] + raPole[1]*Time  + raPole[2]*Time**2 + raNutPrec,
      //    where
      //    raNutPrec  =  raNutPrec1[0]*sin(sysNutPrec[0][0] + sysNutPrec[0][1]*Time) +
      //                  raNutPrec1[1]*sin(sysNutPrec[1][0] + sysNutPrec[1][1]*Time) + ...
      //                  raNutPrec1[N-1]*sin(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time) +
      // (optional for multiples of nutation precession angles)
      //                  raNutPrec2[0]*sin(2*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                  raNutPrec2[1]*sin(2*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                  raNutPrec2[N-1]*sin(2*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time)) +
      //                  raNutPrecM[0]*sin(M*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                  raNutPrecM[1]*sin(M*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                  raNutPrecM[N-1]*sin(M*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time)) +
      //
      // The general equation for the declination of the pole is
      //
      // decPole  =  p_decPole[0] + p_decPole[1]*Time  + p_decPole[2]*Time**2 + decNutPrec,
      //    where
      //    decNutPrec  =  decNutPrec1[0]*cos(sysNutPrec[0][0] + sysNutPrec[0][1]*Time) +
      //                   decNutPrec1[1]*cos(sysNutPrec[1][0] + sysNutPrec[1][1]*Time) + ...
      //                   decNutPrec1[N-1]*cos(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time) +
      //                   decNutPrec2[0]*cos(2*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                   decNutPrec2[1]*cos(2*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                   decNutPrec2[N-1]*cos(2*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time)) +
      // (optional for multiples of nutation precession angles)
      //                   decNutPrecM[0]*sin(M*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                   decNutPrecM[1]*sin(M*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                   decNutPrecM[N-1]*sin(M*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time))
      //
      //     and Time is julian centuries since J2000.
      //
      // The general equation for the prime meridian rotation is
      //
      // pm  =  p_pm[0] + p_pm[1]*Dtime  + p_pm[2]*Dtime**2 + pmNutPrec,
      //    where
      //    pmNutPrec  =  pmNutPrec1[0]*sin(sysNutPrec[0][0] + sysNutPrec[0][1]*Time) +
      //                  pmNutPrec1[1]*sin(sysNutPrec[1][0] + sysNutPrec[1][1]*Time) + ...
      //                  pmNutPrec1[N-1]*sin(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time) +
      // (optional for multiples of nutation precession angles)
      //                  pmNutPrec2[0]*sin(2*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                  pmNutPrec2[1]*sin(2*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                  pmNutPrec2[N-1]*sin(2*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time)) +
      //                  pmNutPrecM[0]*sin(M*(sysNutPrec[0][0] + sysNutPrec[0][1]*Time)) +
      //                  pmNutPrecM[1]*sin(M*(sysNutPrec[1][0] + sysNutPrec[1][1]*Time)) + ...
      //                  pmNutPrecM[N-1]*sin(M*(sysNutPrec[N-1][0] + sysNutPrec[N-1][1]*Time))
      //
      //     Time is interval in Julian centuries since the standard epoch,
      //     dTime is interval in days from the standard epoch (J2000),
      //
      //     N is the number of nutation/precession terms for the planetary system of the target
      //     body,  (possibly including multiple angles as unique terms,
      //             ie. 2*sysNutPrec[0][0] + sysNutPrec[][1]*Time).
      //
      //     Many of the constants in this equation are 0. for a given body.
      //
      //     M is included as an option for future improvements.  M = highest multiple (period)
      //     of any of the nutation/precession angles included in the equations.
      //
      //     ***NOTE*** Currently Naif stores multiples (amplitudes) as if they were additional
      //                nutation/precession terms (periods) in the equation.  This method works as
      //                long as jigsaw does not solve for those values.  In order to solve for
      //                those values, the multiples will need to be known so that the partial
      //                derivatives can be correctly calculated.  Some possible ways of doing this
      //                are 1) Convince Naif to change their data format indicating the relation
      //                      2) Make an Isis version of the PCK data and have Isis software to
      //                          calculate the rotation and partials.
      //                      3) Have an Isis addendum file that identifies the repeated periods
      //                          and software to apply them when calculating the rotation and partials.
      //
      //                For now this software will handle any terms with the same period and different
      //                amplitudes as unique terms in the equation (raNutPrec, decNutPrec,
      //                and pmNutPrec).
      //
      // The next three vectors will have length 3 (for a quadratic polynomial) if used.
      std::vector<Angle>m_raPole;       //!< Coefficients of a quadratic polynomial fitting pole ra.
      std::vector<Angle>m_decPole;      //!< Coefficients of a quadratic polynomial fitting pole dec.
      std::vector<Angle>m_pm ;          //!< Coefficients of a quadratic polynomial fitting pole pm.
      //
      // Currently multiples (terms with periods matching other terms but varying amplitudes)
      // are handled as additional terms added to the end of the vector as Naif does (see
      // comments in any of the standard Naif PCK.
      std::vector<double>m_raNutPrec;    //!< Coefficients of pole right ascension nut/prec terms.
      std::vector<double>m_decNutPrec;  //!< Coefficients of pole decliniation nut/prec terms.
      std::vector<double>m_pmNutPrec;   //!< Coefficients of prime meridian nut/prec terms.

      // The periods of bodies in the same system are modeled with a linear equation
      std::vector<Angle>m_sysNutPrec0; //!< Constants of planetary system nut/prec periods
      std::vector<Angle>m_sysNutPrec1; //!< Linear terms of planetary system nut/prec periods

      // The following scalars are used in the IAU equations to convert p_et to the appropriate time
      // units for calculating target body ra, dec, and w.  These need to be initialized in every
      // constructor.
      //! Seconds per Julian century for scaling time in seconds
      static const double m_centScale;
      //! Seconds per day for scaling time in seconds to get target body w
      static const double m_dayScale;
    
      virtual void SetFullCacheParameters(double startTime, double endTime, int cacheSize);
      virtual void setEphemerisTimeMemcache();
      virtual void setEphemerisTimeNadir();
      virtual void setEphemerisTimeSpice();
      virtual void setEphemerisTimePolyFunction();
      virtual void setEphemerisTimePolyFunctionOverSpice();
      virtual void setEphemerisTimePckPolyFunction();
      std::vector<double> p_cacheTime;  //!< iTime for corresponding rotation
      std::vector<std::vector<double> > p_cache; /**< Cached rotations, stored as
                                                      rotation matrix from J2000
                                                      to 1st constant frame (CJ) or
                                                      coefficients of polynomial
                                                      fit to rotation angles.*/

  };
};

#endif
