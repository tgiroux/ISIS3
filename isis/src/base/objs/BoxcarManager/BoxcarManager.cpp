/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:06 $
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

#include "BoxcarManager.h"

using namespace std;
namespace Isis {
  /**
   * Constructs a BoxcarManager object
   *
   * @param cube The cube this buffer will be associated with.
   *
   * @param boxSamples The number of samples in each boxcar buffer.
   *
   * @param boxLines The number of lines in each boxcar buffer.
   */
  BoxcarManager::BoxcarManager(const Isis::Cube &cube,
                               const int &boxSamples, const int &boxLines) :
    Isis::BufferManager(cube.sampleCount(), cube.lineCount(),
                        cube.bandCount(), boxSamples, boxLines, 1,
                        cube.pixelType()) {

    Isis::BufferManager::SetIncrements(1, 1, 1);
    int soff, loff, boff;
    soff = (int)((boxSamples - 1) / 2) * -1;
    loff = (int)((boxLines - 1) / 2) * -1;
    boff = 0;
    Isis::BufferManager::SetOffsets(soff, loff, boff);
  }
} // end namespace isis

