#include "PositionFactory.h"

namespace Isis {

  SpicePosition* PositionFactory::spicePosition(int targetCode, int observerCode) {
    return new SpicePosition(targetCode, observerCode);
  }

  SpacecraftPosition* PositionFactory::spacecraftPosition(int targetCode, int observerCode,
                     const LightTimeCorrectionState &ltState,
                     const Distance &radius) {
    return new SpacecraftPosition(targetCode, observerCode, ltState, radius);
  }

  Position* PositionFactory::positionSpice(int targetCode, int observerCode) {
    return new PositionSpice(targetCode, observerCode);
  }

  Position* PositionFactory::fromSpiceToMemCache(Position *positionSpice, int time) {
    return fromSpiceToMemCache(positionSpice, time, time, 1);
  }

  Position* PositionFactory::fromSpiceToMemCache(Position *positionSpice, double startTime, double endTime, int size) {
    PositionMemCache *positionMemCache = new PositionMemCache(positionSpice->getTargetCode(), positionSpice->getObserverCode());
    std::vector<std::vector<double>> ephemerisData;
    std::vector<double> cacheTimes;

    cacheTimes = positionSpice->LoadTimeCache(startTime, endTime, size);
    positionMemCache->setStartTime(startTime);
    positionMemCache->setEndTime(endTime);
    positionMemCache->setSize(size);
    positionMemCache->setHasVelocity(positionSpice->getHasVelocity());

    // Loop and load the cache
    for(int i = 0; i < size; i++) {
      double et = cacheTimes[i];
      positionSpice->SetEphemerisTime(et);
      positionMemCache->addCacheCoordinate(positionSpice->Coordinate());
      positionMemCache->addCacheTime(et);
      if (positionMemCache->getHasVelocity()) {
        positionMemCache->addCacheVelocity(positionSpice->Velocity());
      }
    }

    positionMemCache->setSource(Position::Memcache);

    return positionMemCache;
  }

  Position* PositionFactory::loadCache(Table &positionEphemerids) {
    // Load the full cache time information from the label if available
    std::cout << "IN LoadCache" << '\n';
    PositionMemCache* positionMemCache = new PositionMemCache();

    for (int r = 0; r < positionEphemerids.Records(); r++) {
      TableRecord &rec = positionEphemerids[r];
      if (rec.Fields() == 7) {
        positionMemCache->setHasVelocity(true);
      }
      else if (rec.Fields() == 4) {
        positionMemCache->setHasVelocity(false);
      }
      else  {
        QString msg = "Expecting four or seven fields in the Position table";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }

      std::vector<double> j2000Coord;
      j2000Coord.push_back((double)rec[0]);
      j2000Coord.push_back((double)rec[1]);
      j2000Coord.push_back((double)rec[2]);
      int inext = 3;

      positionMemCache->addCacheCoordinate(j2000Coord);
      if (positionMemCache->HasVelocity()) {
        std::vector<double> j2000Velocity;
        j2000Velocity.push_back((double)rec[3]);
        j2000Velocity.push_back((double)rec[4]);
        j2000Velocity.push_back((double)rec[5]);
        inext = 6;

        positionMemCache->addCacheVelocity(j2000Velocity);
      }
      positionMemCache->addCacheTime((double)rec[inext]);
    }

    return positionMemCache;

  }

  Position* PositionFactory::loadCache(json &isdPos) {
    // Load the full cache time information from the label if available
    PositionMemCache* positionMemCache = new PositionMemCache();

    // Load the full cache time information from the label if available
    // p_fullCacheStartTime = isdPos["SpkTableStartTime"].get<double>();
    // p_fullCacheEndTime = isdPos["SpkTableEndTime"].get<double>();
    // p_fullCacheSize = isdPos["SpkTableOriginalSize"].get<double>();
    positionMemCache->setCacheTime(isdPos["EphemerisTimes"].get<std::vector<double>>());

    for (auto it = isdPos["Positions"].begin(); it != isdPos["Positions"].end(); it++) {
      std::vector<double> pos = {it->at(0).get<double>(), it->at(1).get<double>(), it->at(2).get<double>()};
      positionMemCache->addCacheCoordinate(pos);
    }

    bool hasVelocityKey = isdPos.find("Velocities") != isdPos.end();
    positionMemCache->setHasVelocity(hasVelocityKey);

    if (hasVelocityKey) {
      for (auto it = isdPos["Velocities"].begin(); it != isdPos["Velocities"].end(); it++) {
        std::vector<double> vel = {it->at(0).get<double>(), it->at(1).get<double>(), it->at(2).get<double>()};
        positionMemCache->addCacheVelocity(vel);
      }
    }

    // positionMemCache->setHasVelocity(!p_cacheVelocity.empty());

    positionMemCache->SetEphemerisTime(positionMemCache->getCacheTime()[0]);

    return positionMemCache;
  }

}
