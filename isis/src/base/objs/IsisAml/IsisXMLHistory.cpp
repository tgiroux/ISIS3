/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:07 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */


#include <string>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>

#include "IsisXMLHistory.h"
#include "IsisXMLChTrans.h"

using namespace std;

namespace XERCES = XERCES_CPP_NAMESPACE;


// Constructors

IsisXMLHistory::IsisXMLHistory(char *PencodingName,
                               bool &PexpandNamespaces,
                               XERCES::SAX2XMLReader* &Pparser,
                               std::vector<IsisChangeData> *Pchanges) {

  encodingName = PencodingName;
  expandNamespaces = PexpandNamespaces;
  parser = Pparser;
  changes = Pchanges;

  prevDocHandler = parser->getContentHandler();
  prevErrorHandler = parser->getErrorHandler();

  parser->setContentHandler(this);
  parser->setErrorHandler(this);

  generalHandler = NULL;
  ignoreHandler = NULL;
}

IsisXMLHistory::~IsisXMLHistory() {
  if(generalHandler != NULL) delete generalHandler;
  if(ignoreHandler != NULL) delete ignoreHandler;
}


//  IsisXMLHistory: Overrides of the SAX DocumentHandler interface
void IsisXMLHistory::characters(const XMLCh *const chars,
                                const XMLSize_t length) {}


void IsisXMLHistory::endElement(const XMLCh *const uri,
                                const XMLCh *const localname,
                                const XMLCh *const qname) {
  parser->setContentHandler(prevDocHandler);
  parser->setErrorHandler(prevErrorHandler);
}


void IsisXMLHistory::startElement(const XMLCh *const uri,
                                  const XMLCh *const localname,
                                  const XMLCh *const qname,
                                  const XERCES::Attributes &attributes) {

  if((string)XERCES::XMLString::transcode(localname) == (string)"change") {
    if(generalHandler != NULL) {
      delete generalHandler;
      generalHandler = NULL;
    }

    // Pull the attributes out and save them
    unsigned int index = changes->size();
    changes->resize(index + 1);

    // Get the name and date attributes
    string st;
    for(unsigned int i = 0; i < 2; i++) {
      st = XERCES::XMLString::transcode(attributes.getQName(i));
      if(st == "name") {
        (*changes)[index].name = XERCES::XMLString::transcode(attributes.getValue(i));
      }
      else if(st == "date") {
        (*changes)[index].date = XERCES::XMLString::transcode(attributes.getValue(i));
      }
    }

    generalHandler = new IsisXMLHandler(encodingName, expandNamespaces,
                                        parser, &(*changes)[index].description);
  }
  else {
    if(ignoreHandler != NULL) {
      delete ignoreHandler;
      ignoreHandler = NULL;
    }
    ignoreHandler = new IsisXMLIgnore(encodingName, expandNamespaces, parser,
                                      (string)XERCES::XMLString::transcode(localname));
  }

}
