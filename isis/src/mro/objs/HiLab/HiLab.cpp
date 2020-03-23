/**
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

#include "HiLab.h"
#include "IException.h"

using namespace std;

namespace Isis {
  /**
   * Constructs a HiLab Object
   * @param cube The cube containing the HiRise labels to be processed. 
   */
  HiLab::HiLab(Cube *cube) {
    PvlGroup group = cube->group("Instrument");
    p_cpmmNumber = group["CpmmNumber"];
    p_channel = group["ChannelNumber"];
  
    if(group.hasKeyword("Summing")) {
      p_bin = group["Summing"];
    }
    else {
      std::string msg = "Cannot find required Summing keyword in label";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  
    if(group.hasKeyword("Tdi")) {
      p_tdi = group["Tdi"];
    }
    else {
      std::string msg = "Cannot find required Tdi keyword in label";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }

  int HiLab::getCcd() {
    const int cpmm2ccd[] = {0, 1, 2, 3, 12, 4, 10, 11, 5, 13, 6, 7, 8, 9};
    return cpmm2ccd[p_cpmmNumber];
  }
}
