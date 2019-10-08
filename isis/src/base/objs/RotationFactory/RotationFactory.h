#ifndef RotationFactory_h
#define RotationFactory_h
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

#include "EphemerisRotation.h"
#include "SpiceRotation.h"
#include "Rotation.h"

namespace Isis {
  static Rotation* toClassicRotation(EphemerisRotation rotation);
  static Rotation* create(json &isdRotation);
  static Rotation* create(double time);
  static Rotation* create(double startTime, double endTime, int size);
  static Rotation* create(json isdRotation);
  static Rotation* create(Table &table);
}
#endif
