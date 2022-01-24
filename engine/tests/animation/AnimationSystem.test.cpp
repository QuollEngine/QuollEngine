#include "core/Base.h"
#include "animation/AnimationSystem.h"

#include <gtest/gtest.h>

class AnimationSystemTest : public ::testing::Test {
public:
  liquid::EntityContext context;
  liquid::AnimationSystem system;

  AnimationSystemTest() : system(context) {}

  liquid::Entity createEntity(bool loop,
                              const liquid::String &animName = "testAnim") {
    auto entity = context.createEntity();
    context.setComponent<liquid::TransformComponent>(entity, {});
    context.setComponent<liquid::AnimationComponent>(entity, {animName, loop});

    return entity;
  }

  void createAnimation(liquid::KeyframeSequenceTarget target, float time) {
    liquid::Animation animation("testAnim", time);
    liquid::KeyframeSequence sequence(
        target, liquid::KeyframeSequenceInterpolation::Step);

    sequence.addKeyframe(0.0f, glm::vec4(0.0f));
    sequence.addKeyframe(0.5f, glm::vec4(0.5f));
    sequence.addKeyframe(1.0f, glm::vec4(1.0f));

    animation.addKeyframeSequence(sequence);
    system.addAnimation(animation);
  }
};

TEST_F(AnimationSystemTest,
       DoesNotAdvanceEntityAnimationIfAnimationDoesNotExist) {
  auto entity = createEntity(false, "non-existent");
  const auto &animation =
      context.getComponent<liquid::AnimationComponent>(entity);

  EXPECT_EQ(animation.currentTime, 0.0f);
  system.update(2.0f);
  EXPECT_EQ(animation.currentTime, 0.0f);
}

TEST_F(AnimationSystemTest, AdvancedEntityAnimationByDeltaTime) {
  createAnimation(liquid::KeyframeSequenceTarget::Position, 2.0f);
  auto entity = createEntity(false);

  const auto &animation =
      context.getComponent<liquid::AnimationComponent>(entity);
  EXPECT_EQ(animation.currentTime, 0.0f);
  system.update(0.5f);
  EXPECT_EQ(animation.currentTime, 0.5f);
}

TEST_F(AnimationSystemTest, PausesEntityAnimationWhenItReachesAnimationEnd) {
  createAnimation(liquid::KeyframeSequenceTarget::Position, 1.0f);
  auto entity = createEntity(false);

  const auto &animation =
      context.getComponent<liquid::AnimationComponent>(entity);
  EXPECT_EQ(animation.currentTime, 0.0f);
  system.update(1.5f);
  EXPECT_EQ(animation.currentTime, 1.0f);
  system.update(1.5f);
  EXPECT_EQ(animation.currentTime, 1.0f);
}

TEST_F(AnimationSystemTest, RestartsAnimationTimeIfLoop) {
  createAnimation(liquid::KeyframeSequenceTarget::Position, 1.0f);
  auto entity = createEntity(true);
  const auto &animation =
      context.getComponent<liquid::AnimationComponent>(entity);
  EXPECT_EQ(animation.currentTime, 0.0f);
  system.update(1.0f);
  EXPECT_EQ(animation.currentTime, 0.0f);
}

TEST_F(AnimationSystemTest, UpdateEntityPositionBasedOnPositionKeyframe) {
  createAnimation(liquid::KeyframeSequenceTarget::Position, 1.0f);
  auto entity = createEntity(false);

  const auto &transform =
      context.getComponent<liquid::TransformComponent>(entity);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
  system.update(0.5f);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.5f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest, UpdateEntityRotationBasedOnRotationKeyframe) {
  createAnimation(liquid::KeyframeSequenceTarget::Rotation, 1.0f);
  auto entity = createEntity(false);

  const auto &transform =
      context.getComponent<liquid::TransformComponent>(entity);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
  system.update(0.5f);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(0.5f, 0.5f, 0.5f, 0.5f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest, UpdateEntityScaleBasedOnScaleKeyframe) {
  createAnimation(liquid::KeyframeSequenceTarget::Scale, 1.0f);
  auto entity = createEntity(false);

  const auto &transform =
      context.getComponent<liquid::TransformComponent>(entity);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
  system.update(0.5f);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(0.5f));
}
