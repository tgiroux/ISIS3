#ifndef ProcessImportVicar_h
#define ProcessImportVicar_h
/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/12/17 21:13:17 $
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

#include "ProcessImport.h"

namespace Isis {
  /**
   * @brief Import a Vicar file
   *
   * This class allows a programmer to develop application programs which import
   * Vicar cubes and mangles the the Vicar labels into appropriate ISIS labels.
   *
   * Here is an example of how to use ProcessImportVicar
   * @code
   *   ImportVicar p;
   *   Pvl inlab;
   *   p.SetVicarFile("test.vic",inlab);
   *   CubeInfo *opack = p.SetOutputCube("TO");
   *   p.StartProcess();
   *   // extract vicar keywords ...
   *   Pvl outLab;
   *   outLab.addGroup("VICAR");
   *   outLab.addKeyword("RecordSize")",inlab.GetInteger("RECSIZ"); opack->addGroup(outLab,"VICAR");
   *   p.EndProcess();
   * @endcode
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2003-02-13 Jeff Anderson
   *
   * @internal
   *  @history 2005-02-11 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2008-06-26 Christopher Austin - Added the termination char to
   *           SetVicarFile's buf
   *  @history 2011-01-27 Jai Rideout - Fixed to handle VICAR files that
   *           have end labels
   *  @history 2016-04-21 Makayla Shepherd - Added UnsignedWord pixel type handling.
   *
   */

  class ProcessImportVicar : public ProcessImport {

    public:
      void SetVicarFile(const QString &vicarFile, Pvl &vicarLab);

    private:
      QString ExtractPvlLabel(int startPos, std::ifstream &vicarFile) const;
  };
};

#endif


