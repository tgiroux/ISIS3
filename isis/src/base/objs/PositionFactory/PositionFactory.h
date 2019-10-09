#ifndef PositionBuilder_h
#define PositionBuilder_h

#include "Position.h"
#include "PositionSpice.h"
#include "PositionMemCache.h"
#include "SpicePosition.h"
#include "SpacecraftPosition.h"
#include "LightTimeCorrectionState.h"
#include "Distance.h"
#include "IException.h"

namespace Isis {

  class PositionFactory {

    public:
      PositionFactory();

      ~PositionFactory();

      static SpicePosition* spicePosition(int targetCode, int observerCode);

      static SpacecraftPosition* spacecraftPosition(int targetCode, int observerCode,
                              const LightTimeCorrectionState &ltState,
                              const Distance &radius);

      static Position* positionSpice(int targetCode, int observerCode);

      static Position* fromSpiceToMemCache(Position *positionSpice, int time);

      static Position* fromSpiceToMemCache(Position *positionSpice, double startTime, double endTime, int size);

      static Position* loadCache(json &isdPos);

      static Position* loadCache(Table &positionEphemerids);
  };
}

#endif
