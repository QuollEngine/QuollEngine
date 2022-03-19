#pragma once

#include "liquid/entity/EntityContext.h"
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
   * @return Animation index
   */
  uint32_t addAnimation(const Animation &animation);

  /**
   * @brief Update all animations
   *
   * @param dt Time delta
   */
  void update(float dt);

  /**
   * @brief Get animation
   *
   * @param index Animation index
   * @return Animation
   */
  inline const Animation &getAnimation(uint32_t index) const {
    LIQUID_ASSERT(index < mAnimations.size(), "Animation at index " +
                                                  std::to_string(index) +
                                                  " does not exist");
    return mAnimations.at(index);
  }

private:
  EntityContext &mEntityContext;

  std::vector<Animation> mAnimations;
};

} // namespace liquid
