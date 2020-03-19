#include "Isis.h"

#include "getsn.h"
#include "PvlGroup.h"
#include "Pvl.h"

using namespace Isis;
using namespace std;

void IsisMain() {

  // Set Preferences to always turn off Terminal Output
  PvlGroup &grp = Isis::Preference::Preferences().findGroup("SessionLog", Isis::Pvl::Traverse);
  grp["TerminalOutput"].setValue("Off");

  UserInterface &ui = Application::GetUserInterface();
  Pvl appLog;

  getsn(ui, &appLog);

  PvlGroup results = appLog.findGroup("Results");

  if( ui.WasEntered("TO") && ui.IsInteractive() ) {
    Application::GuiLog(results);
  }

  SessionLog::TheLog().AddResults(results);
  
}
