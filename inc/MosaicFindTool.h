#ifndef MosaicFindTool_h
#define MosaicFindTool_h

#include "MosaicTool.h"

class QAction;
class QDialog;
class QGraphicsEllipseItem;
class QLineEdit;
class QToolButton;

namespace Isis {
  class PvlObject;
  /**
   * @brief This controls the 'Find' abilities in the MosaicSceneWidget 
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2011-05-07 Steven Lambright - Refactored along with all of
   *                       'qmos'
   *   @history 2011-05-11 Steven Lambright - Added project settings
   *   @history 2011-09-27 Steven Lambright - Improved user documentation
   *   @history 2012-04-16 Jeannie Backer - Added forward declaration for
   *                           PvlObject.
   */
  class MosaicFindTool : public MosaicTool {
      Q_OBJECT

    public:
      MosaicFindTool(MosaicSceneWidget *);
      void addToMenu(QMenu *menu);

      PvlObject toPvl() const;
      void fromPvl(const PvlObject &obj);
      QString projectPvlObjectName() const;

    protected slots:
      void updateTool();

    protected:
      QAction *getPrimaryAction();
      QWidget *getToolBarWidget();
      QWidget *createToolBarWidget();
      void mouseButtonRelease(QPointF, Qt::MouseButton);

    public slots:
      void getUserGroundPoint();
      void clearPoint();

    private:
      QAction *p_findPoint;
      QToolButton *p_findPtButton;
      QLineEdit *p_status;
      bool p_released;
      bool p_pressed;
      bool p_paint;
      QLineEdit *p_lonLineEdit; //!< Input for longitude
      QLineEdit *p_latLineEdit; //!< Input for latitude
      QGraphicsEllipseItem *p_findSpot;
      QAction *p_action;
  };
};

#endif

