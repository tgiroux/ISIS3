#include <iostream>
#include <sstream>
#include <QString>

#include "getsn.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "SpecialPixel.h"

#include "gmock/gmock.h"


using namespace Isis;


TEST_F(DefaultCube, FunctionalTestGetsnDefault) {

  Pvl appLog;

  getsn( testCube, &appLog );


  PvlGroup results = appLog.findGroup("Results");

  
  // expected sn, on, and filename must be equal
  
  EXPECT_EQ( results.findKeyword("SerialNumber"), "foo" );
  
  EXPECT_EQ( results.findKeyword("Filename"), "foo" );

  EXPECT_EQ( results.findKeyword("ObservationNumber"), "foo" );

}


TEST_F(DefaultCube, FunctionalTestGetsnConsoleOutput) {
   
 
} 
 

TEST_F(DefaultCube, FuncionalTestGetsnFlat) {


}

