#include <QAction>
#include <QString>

namespace Isis {
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GuiHelperAction : public QAction {

      Q_OBJECT

    public:

      GuiHelperAction(QObject *parent, const QString &funct);
      ~GuiHelperAction();

    signals:
      void trigger(const QString &funct);

    private slots:
      void retrigger();

    private:
      QString p_funct;
  };
}

