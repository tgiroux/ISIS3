#include <iostream>
#include <cstdlib>
#include "PhotoModel.h"
#include "PhotoModelFactory.h"
#include "AtmosModel.h"
#include "AtmosModelFactory.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "IException.h"
#include "Anisotropic2.h"
#include "Preference.h"

using namespace Isis;

int main() {
  Isis::Preference::Preferences(true);
  double pstd;
  double trans;
  double trans0;
  double transs;
  double sbar;

  std::cout << "UNIT TEST for Anisotropic2 atmospheric function" << std::endl << std::endl;

  PvlGroup algp("Algorithm");
  algp += PvlKeyword("Name", "Lambert");

  PvlObject op("PhotometricModel");
  op.addGroup(algp);

  PvlGroup alga("Algorithm");
  alga += PvlKeyword("Name", "Anisotropic2");
  alga += PvlKeyword("Tau", "0.28");

  PvlObject oa("AtmosphericModel");
  oa.addGroup(alga);

  Pvl pvl;
  pvl.addObject(op);
  pvl.addObject(oa);
  std::cout << pvl << std::endl << std::endl;

  try {
    PhotoModel *pm = PhotoModelFactory::Create(pvl);
    AtmosModel *am = AtmosModelFactory::Create(pvl, *pm);

    am->SetStandardConditions(true);
    am->CalcAtmEffect(0.0, 0.0, 0.0, &pstd, &trans, &trans0, &sbar, &transs);
    am->SetStandardConditions(false);

    std::cout << "Test phase=0.0, incidence=0.0, emission=0.0 (standard conditions) ..." << std::endl;
    std::cout << "Pstd = " << pstd << std::endl;
    std::cout << "Trans = " << trans << std::endl;
    std::cout << "Trans0 = " << trans0 << std::endl;
    std::cout << "Sbar = " << sbar << std::endl << std::endl;

    am->SetAtmosTau(.28);
    am->CalcAtmEffect(86.7226722, 51.7002388, 38.9414439, &pstd, &trans, &trans0, &sbar, &transs);

    std::cout << "Test phase=86.7226722, incidence=51.7002388, emission=38.9414439 ..." << std::endl;
    std::cout << "Pstd = " << pstd << std::endl;
    std::cout << "Trans = " << trans << std::endl;
    std::cout << "Trans0 = " << trans0 << std::endl;
    std::cout << "Sbar = " << sbar << std::endl << std::endl;

    am->CalcAtmEffect(180.0, 90.0, 90.0, &pstd, &trans, &trans0, &sbar, &transs);

    std::cout << "Test phase=180.0, incidence=90.0, emission=90.0 ..." << std::endl;
    std::cout << "Pstd = " << pstd << std::endl;
    std::cout << "Trans = " << trans << std::endl;
    std::cout << "Trans0 = " << trans0 << std::endl;
    std::cout << "Sbar = " << sbar << std::endl << std::endl;
  }
  catch(IException &e) {
    e.print();
  }

  return 0;
}
