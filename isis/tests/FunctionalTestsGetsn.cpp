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
#include "Cube.h"

#include "gmock/gmock.h"

using namespace Isis;


static QString APP_XML = FileName("$ISISROOT/bin/xml/getsn.xml").expanded();
static QString FROM_PARAM = "FROM=data/defaultImage/defaultCube.pvl";

// check for all correct outputs, no other test needs to check for true params, only false
TEST_F(DefaultCube, FunctionalTestGetsnAllTrue) {
  QString expectedFilename = "data/defaultImage/defaultCube.pvl";
  QString expectedSN = "Viking1/VISB/33322515";
  QString expectedON = "Viking1/VISB/33322515";

  QVector<QString> args = {FROM_PARAM,
			   "FILE=TRUE",
                           "SN=TRUE",
                           "OBSERVATION=TRUE"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  getsn( testCube, options, &appLog );
  PvlGroup results = appLog.findGroup("Results");  
  
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, results.findKeyword("Filename"), expectedFilename);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, results.findKeyword("SerialNumber"), expectedSN);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, results.findKeyword("ObservationNumber"), expectedON);
}


// Default Parameters are file=false, sn=true, observation=false
// Set sn=false to test that all false params give expected output
TEST_F(DefaultCube, FunctionalTestGetsnAllFalse) {
  QVector<QString> args = { FROM_PARAM,
			    "SN=FALSE" };
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
  QString fileName = "default.cub";
  QVector<QString> args = { FROM_PARAM,
			    "FILE=TRUE",
                            "DEFAULT=TRUE" };
  UserInterface options(APP_XML, args);
  Pvl appLog;
  Pvl *testLabel = testCube->label();
  testLabel->findObject( "IsisCube" ).deleteGroup( "Instrument" );
  
  getsn( testCube, options, &appLog );
  PvlGroup results = appLog.findGroup("Results");  

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, fileName , results.findKeyword("SerialNumber") );
}


// Test flatfile mode
TEST_F(DefaultCube, FunctionalTestGetsnFlat) {
  QString expectedSN = "Viking1/VISB/33322515";
    
  QFile flatFile(tempDir.path()+"testOut.txt");
  QVector<QString> args = { FROM_PARAM,
			    "FORMAT=FLAT",
                            "TO="+flatFile.fileName() };
  UserInterface options(APP_XML, args);
  Pvl appLog;

  getsn( testCube, options, &appLog );

  QTextStream flatStream(&flatFile);
  while(!flatStream.atEnd()) {
    QString line = flatStream.readLine();
    QStringList fields = line.split(",");

    EXPECT_PRED_FORMAT2(AssertQStringsEqual, fields.value(1), expectedSN);
  }
}


// Test that append true appends to file
TEST_F(DefaultCube, FunctionalTestGetsnAppend) {
  QFile flatFile(tempDir.path()+"testOut.txt");
  QVector<QString> args = { FROM_PARAM,
			    "FORMAT=FLAT",
                            "TO="+flatFile.fileName(),
                            "APPEND=TRUE"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  getsn( testCube, options, &appLog );
  qint64 sizeInitial = flatFile.size();
  getsn( testCube, options, &appLog );
  qint64 sizeFinal = flatFile.size();

  EXPECT_FALSE( sizeInitial == sizeFinal );
}


// Test that append false overwrites file
TEST_F(DefaultCube, FunctionalTestGetsnOverwrite) {
  QFile flatFile(tempDir.path()+"testOut.txt");
  QVector<QString> args = { FROM_PARAM,
			    "FORMAT=FLAT",
                            "TO="+flatFile.fileName(),
                            "APPEND=FALSE"};
  UserInterface options(APP_XML, args);
  Pvl appLog;

  getsn( testCube, options, &appLog );
  qint64 sizeInitial = flatFile.size();
  getsn( testCube, options, &appLog );
  qint64 sizeFinal = flatFile.size();

  EXPECT_TRUE( sizeInitial == sizeFinal );
}

