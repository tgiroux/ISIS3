#ifndef CISSCALFILE_H
#define CISSCALFILE_H
/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2008/08/25 21:58:30 $
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

#include <vector>

#include <QString>

#include "TextFile.h"

namespace Isis {
  /**
   * @brief Extends <b>TextFile</b>  to handle Cassini ISS
   *        calibration files.
   * This class was created as an extension of <b>TextFile</b> to
   *        be able to read in Cassini ISS calibration files used
   *        by the Isis <B>ciss2isis</B> and <B>cisscal</B>
   *        applications. It is able to read PDS style text files
   *        and skip all header info that exists before the tag
   *        <TT>"\begindata"</TT>.
   *
   * @ingroup Cassini-Huygens
   * @author 2008-03-27 Jeannie Walldren 
   * @internal 
   *   @history 2008-03-27 Jeannie Walldren - Original Version.
   *   @history 2011-05-03 Jeannie Walldren - Fixed documentation of ingroup
   *                          name and added Isis disclaimer.
   */
  class CisscalFile : public TextFile {
    public:
      CisscalFile(const QString &filename, const char *openmode = "input",
                  const char *extension = "");
      //!> Destructor closes the text file.
      ~CisscalFile() {
        TextFile::Close();
      }; 
      bool GetLine(QString &line);
    protected:
      bool p_begindataFound; //!> Flag variable indicates whether the tag <code>"\begindata"</code> has been found.
      bool p_GetLine(QString &line);
  };
};
#endif


