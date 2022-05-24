#include "liquid/core/Base.h"
#include "AnimationSystem.h"

namespace liquid {

AnimationSystem::AnimationSystem(AssetRegistry &assetRegistry)
    : mAssetRegistry(assetRegistry) {}

void AnimationSystem::update(float dt, EntityContext &entityContext) {
  LIQUID_PROFILE_EVENT("AnimationSystem::update");
  const auto &animMap = mAssetRegistry.getAnimations();
  entityContext.iterateEntities<LocalTransformComponent, AnimatorComponent>(
      [&entityContext, &animMap, this, dt](Entity entity, auto &transform,
                                           auto &animComp) {
        auto handle = animComp.animations.at(animComp.currentAnimation);

        if (!animMap.hasAsset(handle)) {
          return;
        }

        const auto &animation = animMap.getAsset(handle);

        if (animComp.playing) {
          animComp.normalizedTime = std::min(
              // Divide delta time by animation time
              // to advance time at a constant speed
              animComp.normalizedTime + (dt / animation.data.time), 1.0f);
          if (animComp.loop && animComp.normalizedTime >= 1.0f) {
            animComp.normalizedTime = 0.0f;
          }
        }

        bool hasSkeleton =
            entityContext.hasComponent<SkeletonComponent>(entity);

        for (const auto &sequence : animation.data.keyframes) {
          const auto &value = mKeyframeInterpolator.interpolate(
              sequence, animComp.normalizedTime);

          if (sequence.jointTarget && hasSkeleton) {
            auto &skeleton =
                entityContext.getComponent<SkeletonComponent>(entity);
            if (sequence.target == KeyframeSequenceAssetTarget::Position) {
              skeleton.jointLocalPositions.at(sequence.joint) =
                  glm::vec3(value);
            } else if (sequence.target ==
                       KeyframeSequenceAssetTarget::Rotation) {
              skeleton.jointLocalRotations.at(sequence.joint) =
                  glm::quat(value.w, value.x, value.y, value.z);
            } else if (sequence.target == KeyframeSequenceAssetTarget::Scale) {
              skeleton.jointLocalScales.at(sequence.joint) = glm::vec3(value);
            }
          } else {
            if (sequence.target == KeyframeSequenceAssetTarget::Position) {
              transform.localPosition = glm::vec3(value);
            } else if (sequence.target ==
                       KeyframeSequenceAssetTarget::Rotation) {
              transform.localRotation =
                  glm::quat(value.w, value.x, value.y, value.z);
            } else if (sequence.target == KeyframeSequenceAssetTarget::Scale) {
              transform.localScale = glm::vec3(value);
            }
          }
        }
      });
}

} // namespace liquid