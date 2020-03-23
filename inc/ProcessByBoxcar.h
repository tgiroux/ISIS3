#ifndef ProcessByBoxcar_h
#define ProcessByBoxcar_h
/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:08 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "Process.h"
#include "Buffer.h"

namespace Isis {
  /**
   * @brief Process cubes by boxcar
   *
   * This is the processing class used to move a boxcar through cube data. This
   * class allows only one input cube and one output cube.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2003-01-03 Tracie Sucharski
   *
   * @internal
   *   @history 2003-04-02 Tracie Sucharski - Added unitTest
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2005-02-08 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *   @history 2012-02-24 Steven Lambright - Added Finalize() and ProcessCube()
   *   @history 2015-01-15 Sasha Brownsberger - Added virtual keyword to several
   *                                           functions to ensure successful
   *                                           inheritance between Process and its
   *                                           child classes.  Also made destructor
   *                                           virtual.  References #2215.
   */

  class ProcessByBoxcar : public Isis::Process {

    private:
      bool p_boxsizeSet; //!< Indicates whether the boxcar size has been set
      int p_boxSamples;  //!< Number of samples in boxcar
      int p_boxLines;    //!< Number of lines in boxcar


    public:

      //! Constructs a ProcessByBoxcar object
      ProcessByBoxcar() {
        p_boxsizeSet = false;
      };

      //! Destroys the ProcessByBoxcar object.
      virtual ~ProcessByBoxcar() {};

      void SetBoxcarSize(const int ns, const int nl);

      using Isis::Process::StartProcess;  // make parent functions visable
      virtual void StartProcess(void funct(Isis::Buffer &in, double &out));
      void ProcessCube(void funct(Isis::Buffer &in, double &out)) {
        StartProcess(funct);
      }

      void EndProcess();
      void Finalize();
  };
};

#endif
