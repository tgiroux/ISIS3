#ifndef PositionMemCache_h
#define PositionMemCache_h

#include "Position.h"

namespace Isis {
  class PositionMemCache:public Position {
    public:
      PositionMemCache(int targetCode, int observerCode);

      PositionMemCache();

      // Replicates logic from Position SetEphemerisTimeMemCache
      virtual void SetEphemerisTime(double et);

      void addCacheCoordinate(std::vector<double> coordinate);

      void addCacheVelocity(std::vector<double> velocity);

      void addCacheTime(double time);

      void setCacheTime(std::vector<double> cacheTimes);

      std::vector<double>& getCacheTime();

      // Only put memcach specific in here, let base do the generic
      // virtual Table Cache(const QString &tableName);

      // Used by base to update the labels
      // virtual void GetTableType(Table &table);
  };
};

#endif
