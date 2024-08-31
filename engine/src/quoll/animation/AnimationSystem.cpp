#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/system/SystemView.h"
#include "AnimationSystem.h"
#include "AnimatorEvent.h"

namespace quoll {

void AnimationSystem::prepare(SystemView &view) {
  QUOLL_PROFILE_EVENT("AnimationSystem::prepare");

  auto &entityDatabase = view.scene->entityDatabase;
  for (auto [entity, animator] : entityDatabase.view<Animator>()) {
    const auto &asset = animator.asset.get();

    if (animator.currentState >= asset.states.size()) {
      animator.currentState = asset.initialState;
    }
  }
}

void AnimationSystem::update(f32 dt, SystemView &view) {
  QUOLL_PROFILE_EVENT("AnimationSystem::update");

  auto &entityDatabase = view.scene->entityDatabase;
  for (auto [entity, animator, animatorEvent] :
       entityDatabase.view<Animator, AnimatorEvent>()) {
    const auto &state = animator.asset->states.at(animator.currentState);

    for (auto &transition : state.transitions) {
      if (transition.eventName == animatorEvent.eventName) {
        animator.currentState = transition.target;
        animator.normalizedTime = 0.0f;
        break;
      }
    }
  }

  entityDatabase.destroyComponents<AnimatorEvent>();

  for (auto [entity, transform, animator] :
       entityDatabase.view<LocalTransform, Animator>()) {

    const auto &state = animator.asset->states.at(animator.currentState);

    const auto &animation = state.animation;

    if (!animation) {
      continue;
    }

    if (animator.playing) {
      animator.normalizedTime = std::min(
          // Divide delta time by animation time
          // to advance time at a constant speed
          animator.normalizedTime + (dt * state.speed / animation->time), 1.0f);
      if (animator.normalizedTime >= 1.0f &&
          state.loopMode == AnimationLoopMode::Linear) {
        animator.normalizedTime = 0.0f;
      }
    }

    bool hasSkeleton = entityDatabase.has<Skeleton>(entity);

    for (const auto &sequence : animation->keyframes) {
      if (sequence.jointTarget && hasSkeleton) {
        auto &skeleton = entityDatabase.get<Skeleton>(entity);
        if (sequence.target == KeyframeSequenceAssetTarget::Position) {
          skeleton.jointLocalPositions.at(sequence.joint) =
              KeyframeInterpolator::interpolateVec3(sequence,
                                                    animator.normalizedTime);
        } else if (sequence.target == KeyframeSequenceAssetTarget::Rotation) {
          skeleton.jointLocalRotations.at(sequence.joint) =
              KeyframeInterpolator::interpolateQuat(sequence,
                                                    animator.normalizedTime);
        } else if (sequence.target == KeyframeSequenceAssetTarget::Scale) {
          skeleton.jointLocalScales.at(sequence.joint) =
              KeyframeInterpolator::interpolateVec3(sequence,
                                                    animator.normalizedTime);
        }
      } else {
        if (sequence.target == KeyframeSequenceAssetTarget::Position) {
          transform.localPosition = KeyframeInterpolator::interpolateVec3(
              sequence, animator.normalizedTime);
        } else if (sequence.target == KeyframeSequenceAssetTarget::Rotation) {
          transform.localRotation = KeyframeInterpolator::interpolateQuat(
              sequence, animator.normalizedTime);
        } else if (sequence.target == KeyframeSequenceAssetTarget::Scale) {
          transform.localScale = KeyframeInterpolator::interpolateVec3(
              sequence, animator.normalizedTime);
        }
      }
    }
  }
}

} // namespace quoll
