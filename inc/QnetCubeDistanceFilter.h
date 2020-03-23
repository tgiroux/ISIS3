#ifndef QnetCubeDistanceFilter_h
#define QnetCubeDistanceFilter_h

#include "QnetFilter.h"

class QLineEdit;
class QRadioButton;

namespace Isis {
  class SerialNumberList;

  /**
   * Defines the Distance filter for the QnetNavTool's Cubes
   * section.  The user must enter a Minimum Distance value. This
   * class is designed to remove cubes from the current filtered
   * list that contain no points within the given distance.
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2008-12-03 Jeannie Walldren - Changed variable
   *                          names and labels for clarity. Added
   *                          radio buttons so the user may choose
   *                          a distance in meters or pixels.
   *                          Rewrote filter() method since it did
   *                          not seem to be working properly, to
   *                          increase efficiency and to allow
   *                          filtering for a distance in meters
   *                          or pixels.
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                          method to replace existing filtered
   *                          list with a subset of that list.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor
   *   @history 2010-10-28 Tracie Sucharski - Fixed some include problems caused
   *                          by changes made to the ControlNet,ControlPoint,
   *                          ControlMeasure header files.
   *
   */
  class QnetCubeDistanceFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetCubeDistanceFilter(QnetNavTool *navTool, QWidget *parent = 0);
      virtual void filter();

    private:
      QLineEdit *p_lineEdit;
      QRadioButton *p_pixels;
      QRadioButton *p_meters;
  };
};

#endif
