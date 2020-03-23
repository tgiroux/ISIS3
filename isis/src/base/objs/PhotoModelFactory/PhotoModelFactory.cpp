/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2007/02/20 16:55:12 $
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

#include "PhotoModelFactory.h"
#include "PhotoModel.h"
#include "Plugin.h"
#include "IException.h"
#include "FileName.h"

namespace Isis {
  /**
   * Create a PhotoModel object using a PVL specification.
   * An example of the PVL required for this is:
   *
   * @code
   * Object = PhotometricModel
   *   Group = Algorithm
   *     PhtName/Name = Minnaert
   *     K = 0.7
   *   EndGroup
   * EndObject
   * @endcode
   *
   * There are many other options that can be set via the pvl and are
   * described in other documentation (see below).
   *
   * @param pvl The pvl object containing the specification
   *
   * @see photometricModels.doc
   **/
  PhotoModel *PhotoModelFactory::Create(Pvl &pvl) {
    // Get the algorithm name to create
    PvlGroup &algo = pvl.findObject("PhotometricModel")
                     .findGroup("Algorithm", Pvl::Traverse);

    QString algorithm = "";
    if (algo.hasKeyword("PhtName")) {
      algorithm = algo["PhtName"][0];
    }
    else if (algo.hasKeyword("Name")) {
      algorithm = algo["Name"][0];
    }
    else {
      QString msg = "Keyword [Name] or keyword [PhtName] must ";
      msg += "exist in [Group = Algorithm]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Open the factory plugin file
    Plugin *p = new Plugin;
    FileName f("PhotoModel.plugin");
    if(f.fileExists()) {
      p->read("PhotoModel.plugin");
    }
    else {
      p->read("$ISISROOT/lib/PhotoModel.plugin");
    }

    // Get the algorithm specific plugin and return it
    PhotoModel * (*plugin)(Pvl & pvl);
    plugin = (PhotoModel * ( *)(Pvl & pvl)) p->GetPlugin(algorithm);
    return (*plugin)(pvl);
  }
} // end namespace isis
