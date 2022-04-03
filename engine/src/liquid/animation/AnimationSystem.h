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
   * @param entityContext Entity context
   * @param assetRegistry Asset registry
   */
  AnimationSystem(EntityContext &entityContext, AssetRegistry &assetRegistry);

  /**
   * @brief Update all animations
   *
   * @param dt Time delta
   */
  void update(float dt);

private:
  EntityContext &mEntityContext;
  AssetRegistry &mAssetRegistry;
  KeyframeInterpolator mKeyframeInterpolator;
};

} // namespace liquid
