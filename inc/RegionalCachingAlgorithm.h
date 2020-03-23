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

#ifndef RegionalCachingAlgorithm_h
#define RegionalCachingAlgorithm_h

#include "CubeCachingAlgorithm.h"

namespace Isis {
  /**
   * @ingroup Low Level Cube IO
   *
   * This algorithm recommends chunks to be freed that are not within the last
   *   IO. Once the 10MB limit is reached, it recommends more chunks to free in
   *   order to stay below this limit, as long as the chunks weren't in the
   *   last IO.
   *
   * @author ????-??-?? Jai Rideout and Steven Lambright
   *
   * @internal
   */
  class RegionalCachingAlgorithm : public CubeCachingAlgorithm {
    public:
      /**
       * @see CubeCachingAlgorithm::recommendChunksToFree()
       * @param allocated
       * @param justUsed
       * @param justRequested
       * @return
       */
      virtual CacheResult recommendChunksToFree(
          QList<RawCubeChunk *> allocated, QList<RawCubeChunk *> justUsed,
          const Buffer &justRequested);
  };
}

#endif
