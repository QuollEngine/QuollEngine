#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/asset/AssetRegistry.h"
#include "KeyframeInterpolator.h"

namespace quoll {

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
   * @brief Prepare data
   *
   * @param entityDatabase Entity database
   */
  void prepare(EntityDatabase &entityDatabase);

  /**
   * @brief Update all animations
   *
   * @param dt Time delta
   * @param entityDatabase Entity database
   */
  void update(f32 dt, EntityDatabase &entityDatabase);

private:
  AssetRegistry &mAssetRegistry;
  KeyframeInterpolator mKeyframeInterpolator;
};

} // namespace quoll
