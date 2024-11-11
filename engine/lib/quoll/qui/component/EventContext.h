#pragma once

#include "HitTestResult.h"

namespace qui {

struct EventContext {
  /**
   * Cached hit test result
   *
   * Used to avoid reallocating path vector every frame
   */
  HitTestResult hitTestResult;
  HitTestResult activeHitTestResult;
  HitTestResult hoveredHitTestResult;
};

} // namespace qui
