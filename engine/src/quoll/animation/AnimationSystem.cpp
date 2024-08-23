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

AnimationSystem::AnimationSystem(AssetRegistry &assetRegistry)
    : mAssetRegistry(assetRegistry) {}

void AnimationSystem::createSystemViewData(SystemView &view) {

  view.animation.queryAnimators = view.scene->entityDatabase.query<Animator>();
  view.animation.queryAnimatorEvents =
      view.scene->entityDatabase.query<Animator, AnimatorEvent>();
  view.animation.queryAnimatorsWithTransform =
      view.scene->entityDatabase.query<Animator, LocalTransform>();
}

void AnimationSystem::prepare(SystemView &view) {
  QUOLL_PROFILE_EVENT("AnimationSystem::prepare");

  auto &entityDatabase = view.scene->entityDatabase;

  view.animation.queryAnimators.each([this](Animator &animator) {
    const auto &animatorAsset =
        mAssetRegistry.getAnimators().getAsset(animator.asset);

    if (animator.currentState >= animatorAsset.data.states.size()) {
      animator.currentState = animatorAsset.data.initialState;
    }
  });
}

void AnimationSystem::update(f32 dt, SystemView &view) {
  QUOLL_PROFILE_EVENT("AnimationSystem::update");

  auto &entityDatabase = view.scene->entityDatabase;

  entityDatabase.defer_begin();
  view.animation.queryAnimatorEvents.each([this](flecs::entity entity,
                                                 Animator &animator,
                                                 AnimatorEvent &animatorEvent) {
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

    entity.remove<AnimatorEvent>();
  });
  entityDatabase.defer_end();

  const auto &animMap = mAssetRegistry.getAnimations();
  view.animation.queryAnimatorsWithTransform.each(
      [&](flecs::entity entity, Animator &animator, LocalTransform &transform) {
        const auto &animatorAsset =
            mAssetRegistry.getAnimators().getAsset(animator.asset);

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
              animator.normalizedTime +
                  (dt * state.speed / animation.data.time),
              1.0f);
          if (animator.normalizedTime >= 1.0f &&
              state.loopMode == AnimationLoopMode::Linear) {
            animator.normalizedTime = 0.0f;
          }
        }

        bool hasSkeleton = entity.has<Skeleton>();

        for (const auto &sequence : animation.data.keyframes) {
          if (sequence.jointTarget && hasSkeleton) {
            auto skeleton = entity.get_ref<Skeleton>();
            if (sequence.target == KeyframeSequenceAssetTarget::Position) {
              skeleton->jointLocalPositions.at(sequence.joint) =
                  KeyframeInterpolator::interpolateVec3(
                      sequence, animator.normalizedTime);
            } else if (sequence.target ==
                       KeyframeSequenceAssetTarget::Rotation) {
              skeleton->jointLocalRotations.at(sequence.joint) =
                  KeyframeInterpolator::interpolateQuat(
                      sequence, animator.normalizedTime);
            } else if (sequence.target == KeyframeSequenceAssetTarget::Scale) {
              skeleton->jointLocalScales.at(sequence.joint) =
                  KeyframeInterpolator::interpolateVec3(
                      sequence, animator.normalizedTime);
            }
          } else {
            if (sequence.target == KeyframeSequenceAssetTarget::Position) {
              transform.localPosition = KeyframeInterpolator::interpolateVec3(
                  sequence, animator.normalizedTime);
            } else if (sequence.target ==
                       KeyframeSequenceAssetTarget::Rotation) {
              transform.localRotation = KeyframeInterpolator::interpolateQuat(
                  sequence, animator.normalizedTime);
            } else if (sequence.target == KeyframeSequenceAssetTarget::Scale) {
              transform.localScale = KeyframeInterpolator::interpolateVec3(
                  sequence, animator.normalizedTime);
            }
          }
        }
      });
}

} // namespace quoll
