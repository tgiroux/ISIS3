/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/06/18 18:53:56 $
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

#include "AtmosModelFactory.h"
#include "AtmosModel.h"
#include "Plugin.h"
#include "IException.h"
#include "FileName.h"

namespace Isis {
  /**
   * Create an AtmosModel object using a PVL specification.
   * An example of the PVL required for this is:
   *
   * @code
   * Object = AtmosphericModel
   *   Group = Algorithm
   *     # Use 'AtmName' instead of 'Name' if using the Gui combo box
   *     # for unique Pvl keyword in DefFile
   *     AtmName/Name = Isotropic1
   *     Tau = 0.7
   *     Tauref = 0.0
   *     Wha = 0.5
   *     Hnorm = 0.003
   *     Nulneg = NO
   *   EndGroup
   * EndObject
   * @endcode
   *
   * There are many other options that can be set via the pvl and are
   * described in other documentation (see below).
   *
   * @param pvl The pvl object containing the specification
   * @param pmodel The PhotoModel objects contining the data
   *
   * @return A pointer to the new AtmosModel
   *
   * @see atmosphericModels.doc
   **/
  AtmosModel *AtmosModelFactory::Create(Pvl &pvl, PhotoModel &pmodel) {

    // Get the algorithm name to create
    PvlGroup &algo = pvl.findObject("AtmosphericModel")
                     .findGroup("Algorithm", Pvl::Traverse);

    QString algorithm = "";
    if(algo.hasKeyword("AtmName")) {
      algorithm = QString(algo["AtmName"]);
    }
    else if(algo.hasKeyword("Name")) {
      algorithm = QString(algo["Name"]);
    }
    else {
      QString msg = "Keyword [Name] or keyword [AtmName] must ";
      msg += "exist in [Group = Algorithm]";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Open the factory plugin file
    Plugin *p = new Plugin;
    FileName f("AtmosModel.plugin");
    if(f.fileExists()) {
      p->read("AtmosModel.plugin");
    }
    else {
      p->read("$ISISROOT/lib/AtmosModel.plugin");
    }

    // Get the algorithm specific plugin and return it
    AtmosModel * (*plugin)(Pvl & pvl, PhotoModel & pmodel);
    plugin = (AtmosModel * ( *)(Pvl & pvl, PhotoModel & pmodel))
             p->GetPlugin(algorithm);
    return (*plugin)(pvl, pmodel);
  }
} // end namespace isis
