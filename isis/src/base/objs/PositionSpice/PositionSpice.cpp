#include "PositionSpice.h"

namespace Isis {

  PositionSpice::PositionSpice(int targetCode, int observerCode) : Position(targetCode, observerCode) {}

  void PositionSpice::SetEphemerisTime(double et) {
    NaifStatus::CheckErrors();
    p_et = et;

    double state[6];
    bool hasVelocity;
    double lt;

    // NOTE:  Prefer method getter access as some are virtualized and portray
    // the appropriate internal representation!!!
    computeStateVector(getAdjustedEphemerisTime(), getTargetCode(), getObserverCode(),
                       "J2000", GetAberrationCorrection(), state, hasVelocity,
                       lt);

    // Set the internal state
    setStateVector(state, hasVelocity);
    setLightTime(lt);

    NaifStatus::CheckErrors();

    return;
  }
}
