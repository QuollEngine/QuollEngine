#include "quoll/core/Base.h"
#include "AnimationSystem.h"

namespace quoll {

AnimationSystem::AnimationSystem(AssetRegistry &assetRegistry)
    : mAssetRegistry(assetRegistry) {}

void AnimationSystem::update(f32 dt, EntityDatabase &entityDatabase) {
  QUOLL_PROFILE_EVENT("AnimationSystem::update");
  const auto &animMap = mAssetRegistry.getAnimations();

  for (auto [entity, animator, animatorEvent] :
       entityDatabase.view<Animator, AnimatorEvent>()) {
    const auto &state = mAssetRegistry.getAnimators()
                            .getAsset(animator.asset)
                            .data.states.at(animator.currentState);

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

    const auto &animatorAsset =
        mAssetRegistry.getAnimators().getAsset(animator.asset);

    if (animator.currentState >= animatorAsset.data.states.size()) {
      animator.currentState = animatorAsset.data.initialState;
    }

    const auto &state = animatorAsset.data.states.at(animator.currentState);

    auto handle = state.animation;

    if (!animMap.hasAsset(handle)) {
      return;
    }

    const auto &animation = animMap.getAsset(handle);

    if (animator.playing) {
      animator.normalizedTime = std::min(
          // Divide delta time by animation time
          // to advance time at a constant speed
          animator.normalizedTime + (dt / animation.data.time), 1.0f);
      if (animator.normalizedTime >= 1.0f) {
        animator.normalizedTime = 0.0f;
      }
    }

    bool hasSkeleton = entityDatabase.has<Skeleton>(entity);

    for (const auto &sequence : animation.data.keyframes) {
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
