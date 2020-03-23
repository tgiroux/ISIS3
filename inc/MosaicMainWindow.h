#ifndef MosaicMainWindow_h
#define MosaicMainWindow_h

#include <iostream>

#include "MainWindow.h"

class QString;
class QProgressBar;

namespace Isis {
  class Cube;
  class MosaicController;
  class ToolPad;

  /**
   * @brief
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2010-05-10 Christopher Austin - added cnet connectivity
   *                           functionality
   *   @history 2011-08-08 Steven Lambright - Refectored for new qmos. Mosaic
   *                           controller is now always visible.
   *   @history 2011-09-26 Steven Lambright - Command line arguments are now
   *                           parsed here.
   *   @history 2011-09-27 Steven Lambright - Improved user documentation
   *   @history 2011-11-21 Steven Lambright - View menu actions come from more
   *                           places now. Fixes #568
   *   @history 2012-03-13 Steven Lambright - m_lastOpenedFile wasn't being
   *                           initialized or set properly causing the file open
   *                           dialogs to always open to '/' which was undesired
   *                           behavior. This has been fixed by initializing it
   *                           to '.' and updating its value  when a cube is
   *                           opened. Fixes #752.
   *   @history 2012-06-18 Steven Lambright - m_lastOpenedFile was being overly
   *                           aggresively when remembering paths from previously
   *                           opened files - it followed the cube paths inside of
   *                           file lists. This is now corrected so that it'll
   *                           remember the path to the file list instead of the
   *                           path to the files in the file list. Fixes #848.
   *   @history 2012-08-28 Tracie Sucharski - Moved the toolpad and active toolbar creation to
   *                           MosaicSceneWidget.
   *   @history 2012-09-17 Steven Lambright - Restored the toolpad and active toolbar creation...
   *                           creation is now in both places, internalization into the scne is for
   *                           ipce only. This allows movement of toolbars/appropriate
   *                           placements of other widgets (progress bar, tracking in status, etc)
   *                           for qmos (aka this class).
   *   @history 2013-03-19 Steven Lambright - Fixed a problem where Open Project would
   *                           close the current project, even if cancelled. Also removed
   *                           the closed project state. Fixes #998.
   *  @history 2017-06-27 Cole Neubauer - Added search capability for file list Dock widgets
   *                           Fixes #1556
   */
  class MosaicMainWindow : public MainWindow {
      Q_OBJECT
    public:
      MosaicMainWindow(QString title, QWidget *parent = 0);
      ~MosaicMainWindow() { }

      QToolBar *permanentToolBar() {
        return m_permToolbar;
      }

      QToolBar *activeToolBar() {
        return m_activeToolbar;
      }

      ToolPad *toolPad() {
        return m_toolpad;
      }

      QProgressBar *progressBar() {
        return m_progressBar;
      }

      /**
      * Returns the View menu.
      *
      *
      * @return QMenu*
      */
      QMenu *viewMenu() const {
        return m_viewMenu;
      };

      void saveSettings();
      void loadProject(QString filename);

    public slots:
      void open();
      void openList();
      void saveProject();
      void saveProjectAs();
      void loadProject();
      void closeMosaic();

    private slots:
      void enterWhatsThisMode();
      void showHelp();
      void updateMenuVisibility();

    protected:
      bool eventFilter(QObject *o, QEvent *e);

    private:
      void readSettings(QSize defaultSize = QSize(800, 600));
      void setupMenus();
      void setupPvlToolBar();
      void saveSettings2();
      void openFiles(QStringList cubeNames);
      bool updateMenuVisibility(QMenu *menu);
      void createController();
      void displayController();

      bool m_controllerVisible;

      ToolPad *m_toolpad; //!< Tool pad on this mainwindow

      QToolBar *m_permToolbar; //!< Tool bar attached to mainwindow
      QToolBar *m_activeToolbar; //!< The active toolbar
      QString m_filename;

      QProgressBar *m_progressBar; //!< The mainwindow's progress bar.

      QMenu *m_viewMenu;
      QMenu *m_settingsMenu;
      QMenu *m_fileMenu;
      QMenu *m_exportMenu;

      MosaicController *m_mosaicController;
      QList<QAction *> m_actionsRequiringOpen;
      QList<QAction *> m_actionsRequiringClosed;
      QList<Cube *> m_openCubes;
      QFileInfo m_lastOpenedFile;
      QSettings m_settings;
      QDockWidget *m_fileListDock;
      QDockWidget *m_mosaicPreviewDock;
  };
};

#endif
