#ifndef Brick_h
#define Brick_h
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

#include "PixelType.h"
#include "BufferManager.h"
#include "Cube.h"

namespace Isis {
  /**
   * @brief  Buffer for containing a three dimensional section of an image
   *
   * This class is a Buffer. The shape of the buffer is three dimensional in the
   * line, sample, and band directions. This class provides a random access
   * window into a cube. The position can be set to any line, sample and band,
   * including outside the image.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2005-01-18 Jeff Anderson
   *
   * @internal
   *  @history 2005-02-25 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *  @history 2006-04-21 Jacob Danton - Modified Brick to extend BufferManager,
   *                                      added new constructor, and two methods,
   *                                      SetBrick and Bricks
   *  @history 2011-11-23 Jai Rideout - Added a new constructor that allows one
   *                                    to specify the shape buffer size as well
   *                                    as the size of the area to be mapped.
   *                                    This is useful for classes such as
   *                                    ProcessByBrick that need to specify an
   *                                    area to be traversed that is bigger than
   *                                    the cube itself.
   *  @history 2017-08-30 Summer Stapleton - Updated documentation. References #4807.
   *
   *  @todo 2005-02-28 Jeff Anderson - add coded and implementation examples to
   *                                   class documentation
   */
  class Brick : public Isis::BufferManager {
    public:
      /**
       * Constructs a Brick object
       *
       * @param nsamps Number of samples in shape buffer
       * @param nlines Number of lines in shape buffer
       * @param nbands Number of bands in shape buffer
       * @param type Type of pixel in raw buffer
       * @param reverse Modifies the order of progression this
       *             buffer takes through the cube.  By default,
       *             progresses samples first, then lines, then bands.
       *             If reverse = true, then the buffer progresses
       *             bands first, then lines, then samples.
       */
      Brick(const int nsamps, const int nlines, const int nbands,
            const Isis::PixelType type, bool reverse=false) :
        Isis::BufferManager(nsamps, nlines, nbands,
                            nsamps, nlines, nbands, type, reverse) {
      };

      /**
       * Constructs a Brick object
       *
       * @param &cube Reference to the cube to calculate a Brick from
       * @param &bufNumSamples Reference to the number of samples in shape buffer
       * @param &bufNumLines Reference to the number of lines in shape buffer
       * @param &bufNumBands Reference to the number of bands in shape buffer
       * @param reverse Modifies the order of progression this
       *             buffer takes through the cube.  By default,
       *             progresses samples first, then lines, then bands.
       *             If reverse = true, then the buffer progresses
       *             bands first, then lines, then samples.
       */
      Brick(const Isis::Cube &cube, const int &bufNumSamples,
            const int &bufNumLines, const int &bufNumBands,
            bool reverse=false) :
        Isis::BufferManager(cube.sampleCount(), cube.lineCount(),
                            cube.bandCount(), bufNumSamples, bufNumLines,
                            bufNumBands, cube.pixelType(), reverse) {
      };

      /**
       * Constructs a Brick object of the specified buffer size and area size to
       * map.
       *
       * @param maxSamples Maximum number of samples to map
       * @param maxLines Maximum number of lines to map
       * @param maxBands Maximum number of bands to map
       * @param bufNumSamples Number of samples in shape buffer
       * @param bufNumLines Number of lines in shape buffer
       * @param bufNumBands Number of bands in shape buffer
       * @param type Type of pixel in raw buffer
       * @param reverse Modifies the order of progression this
       *             buffer takes through the cube.  By default,
       *             progresses samples first, then lines, then bands.
       *             If reverse = true, then the buffer progresses
       *             bands first, then lines, then samples.
       */
      Brick(int maxSamples, int maxLines, int maxBands, int bufNumSamples,
            int bufNumLines, int bufNumBands, Isis::PixelType type,
            bool reverse=false) :
        Isis::BufferManager(maxSamples, maxLines, maxBands, bufNumSamples,
                            bufNumLines, bufNumBands, type, reverse) {
      };

    public:
      /**
       * This method is used to set the base position of the shape buffer.
       * It is used to progress sequentially through a cube by brick.
       *
       * @param start_sample Starting sample to set.
       * @param start_line Starting line to set.
       * @param start_band Starting band to set.
       */
      void SetBasePosition(const int start_sample, const int start_line,
                           const int start_band) {
        this->Isis::Buffer::SetBasePosition(start_sample, start_line, start_band);
      };

      /**
       * This method is used to set the base sample position of the shape buffer.
       *
       * @param start_samp Starting sample to set
       */
      inline void SetBaseSample(const int start_samp) {
        this->Isis::Buffer::SetBaseSample(start_samp);
      };

      /**
       * This method is used to set the base line position of the shape buffer.
       *
       * @param start_line Starting line to set
       */
      inline void SetBaseLine(const int start_line) {
        this->Isis::Buffer::SetBaseLine(start_line);
      };

      /**
       * This method is used to set the base band position of the shape buffer.
       *
       * @param start_band Starting band to set
       */
      inline void SetBaseBand(const int start_band) {
        this->Isis::Buffer::SetBaseBand(start_band);
      };

      void Resize(const int nsamps, const int nlines, const int nbands);

      /**
       * This method is used to set the position of the brick.
       *
       * @param brick Brick number to move to.
       *
       * @return bool Is the brick at the end of the cube?
       */
      bool SetBrick(const int brick);

      /**
       * Returns the number of Bricks in the cube.
       *
       * @return int
       */
      inline int Bricks() {
        return MaxMaps();
      };
  };
};

#endif
