#include "liquid/core/Base.h"
#include "AnimationSystem.h"

namespace liquid {

AnimationSystem::AnimationSystem(EntityContext &entityContext_)
    : entityContext(entityContext_) {}

uint32_t AnimationSystem::addAnimation(const Animation &animation) {
  uint32_t lastId = static_cast<uint32_t>(animations.size());

  animations.push_back(animation);
  return lastId;
}

void AnimationSystem::update(float dt) {
  LIQUID_PROFILE_EVENT("AnimationSystem::update");
  entityContext.iterateEntities<TransformComponent, AnimatorComponent>(
      [this, dt](Entity entity, auto &transform, auto &animComp) {
        uint32_t index = animComp.animations.at(animComp.currentAnimation);

        if (index >= animations.size()) {
          return;
        }

        const Animation &animation = animations.at(index);

        if (animComp.playing) {
          animComp.normalizedTime = std::min(
              // Divide delta time by animation time
              // to advance time at a constant speed
              animComp.normalizedTime + (dt / animation.getTime()), 1.0f);
          if (animComp.loop && animComp.normalizedTime >= 1.0f) {
            animComp.normalizedTime = 0.0f;
          }
        }

        bool hasSkeleton =
            entityContext.hasComponent<SkeletonComponent>(entity);

        for (auto &sequence : animation.getKeyframeSequences()) {
          const auto &value =
              sequence.getInterpolatedValue(animComp.normalizedTime);

          if (sequence.isJointTarget() && hasSkeleton) {
            auto &skeleton =
                entityContext.getComponent<SkeletonComponent>(entity).skeleton;
            if (sequence.getTarget() == KeyframeSequenceTarget::Position) {
              skeleton.setJointPosition(sequence.getJoint(), glm::vec3(value));
            } else if (sequence.getTarget() ==
                       KeyframeSequenceTarget::Rotation) {
              skeleton.setJointRotation(
                  sequence.getJoint(),
                  glm::quat(value.w, value.x, value.y, value.z));
            } else if (sequence.getTarget() == KeyframeSequenceTarget::Scale) {
              skeleton.setJointScale(sequence.getJoint(), glm::vec3(value));
            }
            skeleton.update();
          } else {
            if (sequence.getTarget() == KeyframeSequenceTarget::Position) {
              transform.localPosition = glm::vec3(value);
            } else if (sequence.getTarget() ==
                       KeyframeSequenceTarget::Rotation) {
              transform.localRotation =
                  glm::quat(value.w, value.x, value.y, value.z);
            } else if (sequence.getTarget() == KeyframeSequenceTarget::Scale) {
              transform.localScale = glm::vec3(value);
            }
          }
        }
      });
}

} // namespace liquid