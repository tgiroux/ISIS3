#ifndef SaveProjectAsWorkOrder_H
#define SaveProjectAsWorkOrder_H
/**
 * @file
 * $Revision: 1.19 $
 * $Date: 2010/03/22 19:44:53 $
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
#include "WorkOrder.h"

#include <functional>

#include <QDir>

class QString;

namespace Isis {
  class FileName;

  /**
   * @brief Saves a project to disk (File->Save Project As...)
   *
   * @author 2012-??-?? ???
   *
   * @internal
   *   @history 2017-04-11 Ian Humphrey - Updated the work order according to the redesign of
   *                           WorkOrder. Separated setup and execution steps into setupExecution
   *                           and execution(). Fixes #4763.
   *   @history 2017-07-25 Cole Neubauer - Added project()->setClean call #4969
   *   @history 2017-08-08 Cole Neubauer - Removed project()->setClean call because
   *                            project()->open() calls clear() which changes it appropriately,
   *                            added open call in execution to load the newly saved project #5103
   *   @history 2017-10-20 Cole Neubauer - readded project()->setClean call because
   *                            project()->open() was changed and this needed to be updated to be
   *                            recognized as a clean state #5171
   *
   */
  class SaveProjectAsWorkOrder : public WorkOrder {
      Q_OBJECT
    public:
      SaveProjectAsWorkOrder(Project *project);
      SaveProjectAsWorkOrder(const SaveProjectAsWorkOrder &other);
      ~SaveProjectAsWorkOrder();

      virtual SaveProjectAsWorkOrder *clone() const;

      virtual bool setupExecution();
      virtual void execute();

    private:
      SaveProjectAsWorkOrder &operator=(const SaveProjectAsWorkOrder &rhs);
  };
}

#endif // SaveProjectAsWorkOrder_H
