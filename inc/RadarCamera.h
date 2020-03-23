#ifndef RadarCamera_h
#define RadarCamera_h
/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/08/31 15:11:49 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "Camera.h"

namespace Isis {
  /**
   * @brief Generic class for Radar Cameras
   *
   * This class is used to abstract out radar camera functionality from children
   * classes.
   *  
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author 2009-08-26 Steven Lambright
   *
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added documentation and Isis Disclaimer to files.
   *   @history 2015-09-01 Ian Humphrey and Makayla Shepherd - Modified unit test to override 
   *                           Sensor's pure virtual methods.
   *  
   *   @todo Implement more functionality in this class and abstract away from the children
   */

  class RadarCamera : public Camera {
    public:
      RadarCamera(Cube &cube);

      virtual CameraType GetCameraType() const {
        return Radar;
      }

    private:
      //! Copying cameras is not allowed
      RadarCamera(const RadarCamera &);
      //! Assigning cameras is not allowed
      RadarCamera &operator=(const RadarCamera &);
  };
};

#endif
