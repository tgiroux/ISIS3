#include "Isis.h"

#include "UserInterface.h"
#include "Application.h"
#include "Pvl.h"
#include "getsn.h"


using namespace Isis;
using namespace std;

void IsisMain() {


  // Set Preferences to always turn off Terminal Output
  PvlGroup &grp = Isis::Preference::Preferences().findGroup("SessionLog", Isis::Pvl::Traverse);
  grp["TerminalOutput"].setValue("Off");


  UserInterface &ui = Application::GetUserInterface();

  PvlGroup results = getsn(ui);


  if( (ui.WasEntered("TO")) && (ui.IsInteractive()) ) {
    Application::GuiLog(results);
  }
  else {
    for (int i = 0; i < sn.keywords(); i++) {
      cout << results[i][0] << endl;
    }
  }
  SessionLog::TheLog().AddResults(results);

}
