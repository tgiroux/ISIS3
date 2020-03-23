#ifndef Isis_GuiDoubleParameter_h
#define Isis_GuiDoubleParameter_h

#include <QLineEdit>

#include "GuiParameter.h"


namespace Isis {

//  class QTextEdit;
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class GuiDoubleParameter : public GuiParameter {

      Q_OBJECT

    public:

      GuiDoubleParameter(QGridLayout *grid, UserInterface &ui,
                         int group, int param);
      ~GuiDoubleParameter();

      QString Value();

      void Set(QString newValue);

    private:
      QLineEdit *p_lineEdit;

  };
};



#endif

