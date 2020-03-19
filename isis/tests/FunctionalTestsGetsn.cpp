#include <iostream>
#include <sstream>

#include <QTextStream>
#include <QStringList>
#include <QFile>


#include "getsn.h"
#include "Fixtures.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"


using namespace Isis;

// check for all correct outputs, no other test needs to check for true params, only false
TEST_F(DefaultCube, FunctionalTestGetsnAllTrue) {
  
  // configure UserInterface arguments
  QVector<QString> args = {"FILE=TRUE",
                           "SN=TRUE",
                           "OBSERVATION=TRUE"};
  UserInterface options(APP_XML, args);

  Pvl appLog;

  getsn( testCube, options, &appLog );

  PvlGroup results = appLog.findGroup("Results");

  EXPECT_TRUE( results.findKeyword("Filename") == "data/defaultImage/defaultCube.pvl" );

  EXPECT_TRUE( results.findKeyword("SerialNumber") == "Viking1/VISB/33322515" );
  
  EXPECT_TRUE( results.findKeyword("ObservationNumber") == "Viking1/VISB/33322515" );
  
}


// Default Parameters are file=false, sn=true, observation=false
// Set sn=false to test that all false params give expected output
TEST_F(DefaultCube, FunctionalTestGetsnAllFalse) {
  
  // configure UserInterface arguments
  QVector<QString> args = { "SN=FALSE" };
  UserInterface options(APP_XML, args);

  Pvl appLog;

  getsn( testCube, options, &appLog );

  PvlGroup results = appLog.findGroup("Results");

  EXPECT_FALSE( results.hasKeyword("Filename") );

  EXPECT_FALSE( results.hasKeyword("SerialNumber") );
  
  EXPECT_FALSE( results.hasKeyword("ObservationNumber") );
  
}


// Test the param DEFAULT=TRUE
// when no SN can be generated, the SN should default to Filename
TEST_F(DefaultCube, FunctionalTestGetsnDefaultSn) {
  
  // configure UserInterface arguments
  QVector<QString> args = { "FILE=TRUE",
                            "DEFAULT=TRUE" };
  UserInterface options(APP_XML, args);

  Pvl appLog;

  //   TODO
  // - somehow setup situation where no sn can be generated
  //   ( need to use a different cube ? )

  getsn( testCube, options, &appLog );

  PvlGroup results = appLog.findGroup("Results");  
  EXPECT_TRUE( results.findKeyword("Filename") == results.findKeyword("SerialNumber") );
}


// Test flatfile mode
TEST_F(DefaultCube, FunctionalTestGetsnFlat) {
  QVector<QString> args = { "FORMAT=FLAT" };
  UserInterface options(APP_XML, args);
  
  Pvl appLog;
  getsn( testCube, options, &appLog );

  PvlGroup results = appLog.findGroup("Results");  

  // test flatfile data against expected output:  "Viking1/VISB/33322515"
  // Cube::TemporaryFile
  // use campt as example
  
}
