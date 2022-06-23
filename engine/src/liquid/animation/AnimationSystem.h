#pragma once

#include "liquid/entity/EntityDatabase.h"
#include "liquid/asset/AssetRegistry.h"
#include "KeyframeInterpolator.h"

namespace liquid {

/**
 * @brief Animation system
 */
class AnimationSystem {
public:
  /**
   * @brief Create animation system
   *
   * @param assetRegistry Asset registry
   */
  AnimationSystem(AssetRegistry &assetRegistry);

  /**
   * @brief Update all animations
   *
   * @param dt Time delta
   * @param entityDatabase Entity database
   */
  void update(float dt, EntityDatabase &entityDatabase);

private:
  AssetRegistry &mAssetRegistry;
  KeyframeInterpolator mKeyframeInterpolator;
};

} // namespace liquid
