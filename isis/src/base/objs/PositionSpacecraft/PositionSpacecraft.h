#ifndef PositionSpacecraft_h
#define PositionSpacecraft_h

#include <string>
#include <vector>
#include "QString.h"
#include "Distance.h"
#include "LightTimeCorrectionState.h"
#include "Position.h"

namespace Isis {
  class PositionSpacecraft: public Position {
    public:
      PositionSpacecraft(int targetCode, int observerCode,
                         const LightTimeCorrectionState &ltState = LightTimeCorrectionState(),
                         const Distance &radius = Distance(0.0, Distance::Meters));

      // destructor
      virtual ~PositionSpacecraft() {}

      double getRadiusLightTime() const;
      static double getDistanceLightTime(const Distance &distance);

      virtual void SetAberrationCorrection(const QString &correction);
      virtual QString GetAberrationCorrection() const;

      // Replicates logic from SpaceCraftPosition SetEphemerisTimeSpice
      virtual void SetEphemerisTime(double et);

      const LightTimeCorrectionState &getLightTimeState() const;

    private:
      LightTimeCorrectionState m_abcorr;   //!< Light time correction state
      Distance                 m_radius;   //!< Radius of target
  };
};

#endif
