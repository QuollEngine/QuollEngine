#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/LocalTransform.h"
#include "Animator.h"
#include "AnimatorEvent.h"

namespace quoll {

struct AnimationSystemView {
  flecs::query<Animator> queryAnimators;
  flecs::query<Animator, AnimatorEvent> queryAnimatorEvents;
  flecs::query<Animator, LocalTransform> queryAnimatorsWithTransform;
};

} // namespace quoll
