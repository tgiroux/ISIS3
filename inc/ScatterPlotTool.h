#ifndef ScatterPlotTool_h
#define ScatterPlotTool_h

#include "Tool.h"

#include <QPointer>
#include <QScopedPointer>

class QAction;
template< class T > class QVector;
class QWidget;

namespace Isis {
  class MdiCubeViewport;
  class ScatterPlotConfigDialog;
  class ScatterPlotWindow;

  /**
   * @brief Scatter Plot Tool
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2010-06-26 Eric Hyer - Now uses MdiCubeViewport instead of
   *                           CubeViewport.  Also fixed some include issues.
   */
  class ScatterPlotTool : public Tool {
      Q_OBJECT

    public:
      ScatterPlotTool(QWidget *parent);
      void setActionChecked(bool checked);

      virtual void paintViewport(MdiCubeViewport *vp, QPainter *painter);

    public slots:
      void onScatterPlotConfigAccepted();
      void onScatterPlotConfigRejected();
      void showNewScatterPlotConfig();

    protected slots:
      void mouseMove(QPoint p, Qt::MouseButton);
      void mouseLeave();
      void repaintViewports();

    protected:
      QWidget *createToolBarWidget(QStackedWidget *parent);
      QAction *toolPadAction(ToolPad *pad);

      QAction *toolAction();

    private:
      //! The action which activates this tool.
      QPointer<QAction> m_action;
      //! This is the configuration dialog for a new scatter plot
      QPointer<ScatterPlotConfigDialog> m_configDialog;
      //! This is a list of all of the plot windows used for painting/alarming
      QScopedPointer< QList< QPointer<ScatterPlotWindow> > > m_plotWindows;
  };
};




#endif

