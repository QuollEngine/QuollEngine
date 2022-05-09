#pragma once

#include "liquid/entity/EntityContext.h"
#include "liquid/asset/AssetRegistry.h"
#include "KeyframeInterpolator.h"

namespace liquid {

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
   * @param entityContext Entity context
   */
  void update(float dt, EntityContext &entityContext);

private:
  AssetRegistry &mAssetRegistry;
  KeyframeInterpolator mKeyframeInterpolator;
};

} // namespace liquid
