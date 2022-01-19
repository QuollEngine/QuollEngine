#pragma once

#include "entity/EntityContext.h"
#include "Animation.h"

namespace liquid {

class AnimationSystem {
public:
  /**
   * @brief Create animation system
   *
   * @param entityContext Entity context
   */
  AnimationSystem(EntityContext &entityContext);

  /**
   * @brief Add animation
   *
   * @param animation Animation
   */
  void addAnimation(const Animation &animation);

  /**
   * @brief Update all animations
   *
   * @param dt Time delta
   */
  void update(float dt);

private:
  EntityContext &entityContext;

  std::unordered_map<String, Animation> animations;
};

} // namespace liquid
