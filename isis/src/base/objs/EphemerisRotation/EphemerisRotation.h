#ifndef EphemerisRotation_h
#define EphemerisRotation_h

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
#include "Quaternion.h"

#define J2000Code    1

namespace Isis {

  EphemerisRotation(int frameCode);
  EphemerisRotation(int frameCode, int targetCode);
  EphemerisRotation(const EphemerisRotation &rotToCopy);
  EphemerisRotation::~EphemerisRotation(); 
  
  virtual SetEphemerisTime(double et);
  virtual MinimizeCache(DownsizeStatus status);
  virtual LoadCache(Table &table);
  virtual loadPCFromTable(const PvlObject &label); 
  virtual CacheLabel(Table &table);/
}

#endif
