#include "liquid/core/Base.h"
#include "AnimationSystem.h"

namespace liquid {

AnimationSystem::AnimationSystem(AssetRegistry &assetRegistry)
    : mAssetRegistry(assetRegistry) {}

void AnimationSystem::update(float dt, EntityDatabase &entityDatabase) {
  LIQUID_PROFILE_EVENT("AnimationSystem::update");
  const auto &animMap = mAssetRegistry.getAnimations();
  for (auto [entity, transform, animator] :
       entityDatabase.view<LocalTransform, Animator>()) {
    auto handle = animator.animations.at(animator.currentAnimation);

    if (!animMap.hasAsset(handle)) {
      return;
    }

    const auto &animation = animMap.getAsset(handle);

    if (animator.playing) {
      animator.normalizedTime = std::min(
          // Divide delta time by animation time
          // to advance time at a constant speed
          animator.normalizedTime + (dt / animation.data.time), 1.0f);
      if (animator.loop && animator.normalizedTime >= 1.0f) {
        animator.normalizedTime = 0.0f;
      }
    }

    bool hasSkeleton = entityDatabase.has<Skeleton>(entity);

    for (const auto &sequence : animation.data.keyframes) {
      const auto &value =
          mKeyframeInterpolator.interpolate(sequence, animator.normalizedTime);

      if (sequence.jointTarget && hasSkeleton) {
        auto &skeleton = entityDatabase.get<Skeleton>(entity);
        if (sequence.target == KeyframeSequenceAssetTarget::Position) {
          skeleton.jointLocalPositions.at(sequence.joint) = glm::vec3(value);
        } else if (sequence.target == KeyframeSequenceAssetTarget::Rotation) {
          skeleton.jointLocalRotations.at(sequence.joint) =
              glm::quat(value.w, value.x, value.y, value.z);
        } else if (sequence.target == KeyframeSequenceAssetTarget::Scale) {
          skeleton.jointLocalScales.at(sequence.joint) = glm::vec3(value);
        }
      } else {
        if (sequence.target == KeyframeSequenceAssetTarget::Position) {
          transform.localPosition = glm::vec3(value);
        } else if (sequence.target == KeyframeSequenceAssetTarget::Rotation) {
          transform.localRotation =
              glm::quat(value.w, value.x, value.y, value.z);
        } else if (sequence.target == KeyframeSequenceAssetTarget::Scale) {
          transform.localScale = glm::vec3(value);
        }
      }
    }
  }
}

} // namespace liquid
