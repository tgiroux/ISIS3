/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2008/09/03 16:21:02 $
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

#ifndef CubeTileHandler_h
#define CubeTileHandler_h

#include "CubeIoHandler.h"

namespace Isis {

  /**
   * @brief IO Handler for Isis Cubes using the tile format.
   *
   * This class is used to open, create, read, and write data from Isis cube
   * files.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2003-02-14 Jeff Anderson
   *
   * @internal
   *   @history 2007-09-14 Stuart Sides - Fixed bug where pixels
   *            from a buffer outside the ns/nl were being
   *            transfered to the right most and bottom most tiles
   *   @history 2011-06-16 Jai Rideout and Steven Lambright - Refactored to
   *                           work with the new parent.
   *   @history 2011-07-18 Jai Rideout and Steven Lambright - Added
   *                           unimplemented copy constructor and assignment
   *                           operator.
   */

  class CubeTileHandler : public CubeIoHandler {
    public:
      CubeTileHandler(QFile * dataFile, const QList<int> *virtualBandList,
          const Pvl &label, bool alreadyOnDisk);
      ~CubeTileHandler();

      void updateLabels(Pvl &label);

    protected:
      virtual void readRaw(RawCubeChunk &chunkToFill);
      virtual void writeRaw(const RawCubeChunk &chunkToWrite);

    private:
      /**
       * Disallow copying of this object.
       *
       * @param other The object to copy.
       */
      CubeTileHandler(const CubeTileHandler &other);

      /**
       * Disallow assignments of this object
       *
       * @param other The CubeTileHandler on the right-hand side of the
       *              assignment that we are copying into *this.
       * @return A reference to *this.
       */
      CubeTileHandler &operator=(const CubeTileHandler &other);

      int findGoodSize(int maxSize, int dimensionSize) const;
      BigInt getTileStartByte(const RawCubeChunk &chunk) const;
  };
}

#endif
