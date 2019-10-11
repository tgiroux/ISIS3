#ifndef SpiceRotation_h
#define SpiceRotation_h
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

#include "Rotation.h"

namespace Isis {
  /**
   * @brief Obtain SPICE rotation information for a body
   *
   * This class will obtain the rotation from J2000 to a particular reference
   * frame, for example the rotation from J2000 to MOC NA.
   *
   * It is essentially used to convert position vectors from one frame to
   * another, making it is a C++ wrapper to the NAIF routines pxform_c and
   * mxv or mtxv.  Therefore, appropriate NAIF kernels are expected to be
   * loaded prior to using this class.  A position can be returned in either
   * the J2000 frame or the selected reference frame.  See NAIF required
   * reading for more information regarding this subject at
   * ftp://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/spk.html
   * <p>
   * An important functionality of this class is the ability to cache the
   * rotations so they do not have to be constantly read from the NAIF kernels
   * and they can be more conveniently updated.  Once the data is cached, the
   * NAIF kernels can be unloaded.  If the rotation has a fixed part and a time-
   * based part, the rotation is computed and stored in those two parts.
   *
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2005-12-01 Debbie A. Cook
   *
   * @internal
   *   @history 2005-12-01 Debbie A. Cook Original Version modified from
   *                           SpicePosition class by Jeff Anderson
   *   @history 2006-03-23 Jeff Anderson modified SetEphemerisTime to return
   *                           if the time did not change to improve speed.
   *   @history 2006-10-18 Debbie A. Cook Added method, WrapAngle, to wrap
   *                           angles around 2 pi
   *   @history 2007-12-05 Debbie A. Cook added method SetPolynomialDegree to
   *                           allow the degree of the polynomials fit to the
   *                           camera angles to be changed.  Also changed the
   *                           polynomial from a fixed 2nd order polynomial to
   *                           an nth degree polynomial with one independent
   *                           variable.  PartialType was revised and the calls
   *                           to SetReferencePartial (has an added argument,
   *                           coefficient index) and DPolynomial (argument type
   *                           changed to int) were revised. The function was
   *                           changed from Parabola to Polynomial1Variable, now
   *                           called PolynomialUnivariate. New methods
   *                           GetBaseTime and SetOverrideBaseTime were added
   *   @history 2008-02-15 Debbie A. Cook added a new error message to handle
   *                           the case where the Naif reference frame code is
   *                           not recognized.
   *   @history 2008-06-18 Unknown - Fixed documentation, added NaifStatus calls
   *   @history 2008-11-26 Debbie A. Cook Added method to set axes of rotation.
   *                           Default axes are still 3,1,3 so existing software
   *                           will not be affected by the change.  Also added
   *                           timeScale to the the class and made some
   *                           parameters protected instead of private so they
   *                           are available to inheriting classes.
   *   @history 2008-12-12 Debbie A. Cook Added method to return frame code
   *   @history 2009-01-26 Debbie A. Cook Added wrap of 3rd camera angle when
   *                           crossing +-180
   *   @history 2009-04-21 Debbie A. Cook Added methods MinimizeCache and
   *                           LoadTimeCache, variable p_minimizeCache, and
   *                           enum constants DownsizeStatus
   *   @history 2009-06-29 Debbie A. Cook Fixed memory overwrite problem in
   *                           LoadTimeCache when reading a type 3 ck
   *   @history 2009-07-24 Debbie A. Cook Removed downsizing for Nadir
   *                           instrument pointing tables (LoadTimeCache) so
   *                           that radar instruments will work.  Current
   *                           downsizing code requires sclk and radar has no
   *                           sclk.
   *   @history 2009-10-01 Debbie A. Cook Divided the rotation into a constant
   *                           (in time) part and a time-based part and
   *                           added keywords listing the frame chains for both
   *                           the constant part and the time-based part.
   *   @history 2009-10-09 Debbie A. Cook Added angular velocity when it is
   *                           available
   *   @history 2009-10-30 Unknown - Modified J2000Vector and ReferenceVector
   *                           to work on either length 3 vectors (position
   *                           only) or lenght 6 vectors (position and velocity)
   *                           and added private method StateTJ()
   *   @history 2009-12-03 Debbie A. Cook Modified tests in LoadTimeCache to
   *                           allow observation to cross segment boundary for
   *                           LRO
   *   @history 2010-03-19 Debbie A. Cook Revised ReloadCache including removing
   *                           obsolete arguments.  Added initialization of
   *                           members p_fullCacheStartTime, p_fullCacheEndTime,
   *                           and p_fullCacheSize.  Added these same values to
   *                           the table label in method Cache and the reading
   *                           of these values to the method LoadCache(table).
   *                           Improved error message in FrameTrace.  Also
   *                           corrected a comment in StateTJ
   *   @history 2010-09-23 Debbie A. Cook Revised to write out line cache for
   *                           updated pointing when cache size is 1. If the
   *                           original pointing had an angular velocity in
   *                           this case, the original angular velocity is
   *                           written out along with the updated quaternion.
   *                           Also added method Extrapolate, to extrapolate
   *                           pointing assuming a constant angular velocity.
   *                           This method was designed to compute the pointing
   *                           at the start and end of the exposure for framing
   *                           cameras to create a ck that would cover a single
   *                           framing observation.
   *   @history 2010-12-22 Debbie A. Cook  Added new method
   *                           SetFullCacheParameters to upgrade appjit to
   *                           current instrument Rotation group labels.
   *   @history 2011-02-17 Debbie A. Cook  Fixed bug in method LineCache and
   *                           fixed computation of angular velocity in method
   *                           DCJdt (derivative was with respect to scaled et
   *                           instead of et)
   *   @history 2011-02-22 Debbie A. Cook - Corrected Extrapolation method
   *   @history 2011-03-25 Debbie A. Cook - Added method GetCenterAngles()
   *   @history 2011-07-20 Kris J Becker - Modified
   *                           SpiceRotation::LoadCache(Table &table) to be
   *                           reentrant.  This mod was necessitated by the Dawn
   *                           VIR instrument.
   *   @history 2012-05-28 Debbie A. Cook - Programmer notes - A new
   *                           interpolation algorithm, PolyFunctionOverSpice,
   *                           was added and new supporting methods:
   *                           SetEphemerisTimePolyOverSpice,  SetEphemerisTimeSpice,
   *                           SetEphemerisTimeNadir, SetEphemerisTimeMemcache,
   *                           and SetEphemerisTimePolyFunction.
   *                           PolyFunctionOverSpice is never output, but is
   *                           converted to a line cache and reduced.  Methods
   *                           LineCache and ReloadCache were modified to do the
   *                           reduction and a copy constructor was added to
   *                           support the reduction.  Also an argument was
   *                           added to SetPolynomial methods for function type,
   *                           since PolyFunction is no longer the only function
   *                           supported.  These changes help the BundleAdjust
   *                           applications to better fit line scan images where
   *                           the pointing was not modeled well with a regular
   *                           polynomial.
   *   @history 2012-10-25 Jeannie Backer - Brought class closer to Isis3
   *                           standards: Ordered includes in cpp file, replaced
   *                           quotation marks with angle braces in 3rd party
   *                           includes, fixed history indentation and line
   *                           length. References #1181.
   *   @history 2013-03-27 Jeannie Backer - Added methods for MsiCamera. Brought
   *                           class closer to Isis3 standards: moved method
   *                           implementation to cpp file, fixed documentation.
   *                           References #1248.
   *   @history 2013-11-12 Ken Edmundson Programmers notes - Commented out cout
   *                           debug statements on lines 637 and 642 that appeared
   *                           onscreen during jigsaw runs when images are updated.
   *                           References #1521.
   *   @history 2014-03-11 Tracie Sucharski - In the LoadTimeCache method, do not throw error if
   *                           if first segment in kernel is not type 3 or 5.  As long as the
   *                           segment needed is type 3 or 5, we're ok.  This was changed for
   *                           New Horizons which had ck's with both type 2 and type 3 segments.
   *   @history 2014-03-11 Stuart Sides - Programmers notes - Fixed a bug in the copy constructor
   *                           that was going out of array bounds.
   *   @history 2015-02-20 Jeannie Backer - Improved error messages.
   *   @history 2015-07-21 Kristin Berry - Added additional NaifStatus::CheckErrors() calls to see if
   *                           any NAIF errors were signaled. References #2248.
   *   @history 2015-08-05 Debbie A. Cook - Programmer notes - Modified LoadCache,
   *                           and ComputeAv.
   *                           Added new methods
   *                           loadPCFromSpice, loadPCFromTable, toJ2000Partial, poleRaCoefs,
   *                           poleDecCoefs, pmCoefs, poleRaNutPrecCoefs, poleDecNutPrecCoefs,
   *                           pmNutPrecCoefs, sysNutPrecConstants, sysNutPrecCoefs,
   *                           usePckPolynomial, setPckPolynomial(raCoef, decCoef, pmCoef),
   *                           getPckPolynomial, setEphemerisTimePckPolyFunction, getFrameType
   *                           and members m_frameType, m_tOrientationAvailable,
   *                           m_raPole, m_decPole, m_pm, m_raNutPrec, m_decNutPrec, m_pmNutPrec,
   *                           m_sysNutPrec0, m_sysNutPrec1, m_dscale, m_Tscale to support request for
   *                           solving for target body parameters.
   *                           Also added a new enumerated value for Source, PckPolyFunction,
   *                           and PartialType, WRT_RotationRate.
   *   @history 2016-02-15 Debbie A. Cook - Programmer notes - Added private method
   *                           setFrameType to set the frame type.  It also loads the planetary
   *                           constants for a PCK type.
   *   @history 2016-06-28 Ian Humphrey - Updated documentation and coding standards. Added new
   *                           tests to unit test. Fixes #3972.
   *   @history 2017-12-13 Ken Edmundson - Added "case DYN:" to methods ToReferencePartial and toJ2000Partial. Fixes #5251.
   *                           This problem was found when trying to bundle M3 images that had been spiceinited with nadir
   *                           pointing. The nadir frame is defined as a Dynamic Frame by Naif.
   *   @history 2018-04-21 Jesse Mapel - Modified frametype resolution to check if a body centered
   *                           frame uses a CK or PCK definition. This only occurs for bodies
   *                           where a pck cannot accurately define for the duration of a mission.
   *                           The current example is the comet 67P/CHURYUMOV-GERASIMENKO
   *                           imaged by Rosetta. Some future comet/astroid missions are expected
   *                           to use a CK defined body fixed reference frame. Fixes #5408.
   *
   *  @todo Downsize using Hermite cubic spline and allow Nadir tables to be downsized again.
   *  @todo Consider making this a base class with child classes based on frame type or
   *              storage type (polynomial, polynomial over cache, cache, etc.)
   */
     class SpiceRotation : public Rotation {
      public:
        SpiceRotation(int frameCode) : Rotation(frameCode) {}
        SpiceRotation(int frameCode, int bodyCode) : Rotation(frameCode, bodyCode) {}
       };
};

#endif
