#ifndef HrscCamera_h
#define HrscCamera_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/08/31 15:12:31 $
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

#include "LineScanCamera.h"

#include <QString>

#include "VariableLineScanCameraDetectorMap.h"

namespace Isis {
  /**
   * @brief HRSC Camera Model
   *
   * This class is the implementation of the camera model
   * for the Mars Express HRSC (High/Super Resolution Stereo Colour Imager) 
   * instrument. 
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup MarsExpress
   *
   * @author 2008-07-28 Steven Lambright
   *
   * @internal
   *   @history 2008-08-08 Steven Lambright Made the unit test work with a Sensor
   *                           change. Also, now using the new LoadCache(...) method instead of
   *                           createCache(...). Increased the delta line/samp tolerance in the
   *                           unit test.
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test
   *                           for new methods. Updated documentation. Removed Mex
   *                           namespace wrap inside Isis namespace. Added Isis Disclaimer to
   *                           files. Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2015-08-12 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument 
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   *   @history 2016-10-21 Kristin Berry - Backward Compatibility Issue: Updates to improve
   *                           VariableLineScanCameraDetectorMap's time-accuracy will cause
   *                           HrscCamera to calculate updated (lat,lon) values.
   *                           The unitTest was updated to reflect this. References #4476.
   *   @history 2016-10-21 Kristin Berry - Updated unitTest. References #4476.
   *   @history 2016-10-27 Jeannie Backer - Moved method implementation to cpp file.
   *                           Added some variable documentation. References #4476.
   */
  class HrscCamera : public LineScanCamera {
    public:
      // Constructs a HiriseCamera object
      HrscCamera(Cube &cube);

      // Destroys the HiriseCamera object
      ~HrscCamera();

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;

    private:
      void ReadLineRates(QString filename);

      std::vector<LineRateChange> p_lineRates; /**< Vector of the variable line rates for this 
                                                    camera model.*/
  };
};
#endif
