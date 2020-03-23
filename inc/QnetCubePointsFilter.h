#ifndef QnetCubePointsFilter_h
#define QnetCubePointsFilter_h

#include "QnetFilter.h"

class QRadioButton;
class QLineEdit;

namespace Isis {
  /**
   * Defines the Points filter for the QnetNavTool's Cubes
   * section.  The user may choose to enter a maximum or minimum
   * number of points. This class is designed to remove cubes from
   * the current filtered list that contain more than the maximum
   * or less than the minimum number of points.
   *
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2009-01-08 Jeannie Walldren - Modified filter()
   *                           method to create a new filtered list
   *                           from images in the existing filtered
   *                           list.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                           to null in constructor
   *   @history 2010-10-28 Tracie Sucharski - Fixed some include problems caused
   *                           by changes made to the ControlNet,ControlPoint,
   *                           ControlMeasure header files.
   *
   *
   */
  class QnetCubePointsFilter : public QnetFilter {
      Q_OBJECT

    public:
      QnetCubePointsFilter(QnetNavTool *navTool, QWidget *parent = 0);
      virtual void filter();

    private:
      QRadioButton *m_lessThanRB;
      QRadioButton *m_greaterThanRB;
      QLineEdit *m_pointEdit;
  };
};

#endif
