#include "liquid/core/Base.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/scene/Skeleton.h"

#include <gtest/gtest.h>

class AnimationSystemTest : public ::testing::Test {
public:
  liquid::EntityContext context;
  liquid::AnimationSystem system;
  liquid::AssetRegistry assetRegistry;
  liquid::rhi::ResourceRegistry registry;

  AnimationSystemTest() : system(context, assetRegistry) {}

  liquid::Entity createEntity(
      bool loop,
      liquid::AnimationAssetHandle animIndex = liquid::AnimationAssetHandle{1},
      bool playing = true) {
    auto entity = context.createEntity();
    context.setComponent<liquid::TransformComponent>(entity, {});
    context.setComponent<liquid::AnimatorComponent>(
        entity, {0, loop, 0.0f, playing, {animIndex}});

    return entity;
  }

  liquid::Entity createEntityWithSkeleton(
      bool loop,
      liquid::AnimationAssetHandle animIndex = liquid::AnimationAssetHandle{1},
      bool playing = true) {
    auto entity = createEntity(loop, animIndex, playing);

    liquid::Skeleton skeleton(liquid::SkeletonAssetHandle{2}, {glm::vec3{0.0f}},
                              {glm::quat{1.0f, 0.0f, 0.0f, 0.0f}},
                              {glm::vec3{1.0f}}, {0}, {glm::mat4{1.0f}},
                              {"Joint0"}, &registry);

    context.setComponent<liquid::SkeletonComponent>(entity, {skeleton});

    return entity;
  }

  liquid::AnimationAssetHandle
  createAnimation(liquid::KeyframeSequenceAssetTarget target, float time) {
    liquid::AssetData<liquid::AnimationAsset> animation;
    animation.name = "testAnim";
    animation.data.time = time;

    liquid::KeyframeSequenceAsset sequence;
    sequence.target = target;
    sequence.interpolation = liquid::KeyframeSequenceAssetInterpolation::Step;

    sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
    sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f),
                               glm::vec4(1.0f)};

    animation.data.keyframes.push_back(sequence);

    return assetRegistry.getAnimations().addAsset(animation);
  }

  liquid::AnimationAssetHandle
  createSkeletonAnimation(liquid::KeyframeSequenceAssetTarget target,
                          float time) {
    liquid::AssetData<liquid::AnimationAsset> animation;
    animation.name = "testAnim";
    animation.data.time = time;

    liquid::KeyframeSequenceAsset sequence;
    sequence.target = target;
    sequence.interpolation = liquid::KeyframeSequenceAssetInterpolation::Step;
    sequence.joint = 0;
    sequence.jointTarget = true;

    sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
    sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f),
                               glm::vec4(1.0f)};

    animation.data.keyframes.push_back(sequence);

    return assetRegistry.getAnimations().addAsset(animation);
  }
};

using AnimationSystemDeathTest = AnimationSystemTest;

TEST_F(AnimationSystemTest,
       DoesNotAdvanceEntityAnimationIfAnimationDoesNotExist) {
  auto entity = createEntity(false, liquid::AnimationAssetHandle::Invalid);
  const auto &animation =
      context.getComponent<liquid::AnimatorComponent>(entity);

  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(0.5f);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest, DoesNotAdvanceTimeIfComponentIsNotPlaying) {
  auto animIndex =
      createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = createEntity(false, animIndex, false);

  const auto &animation =
      context.getComponent<liquid::AnimatorComponent>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(0.5f);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest,
       AdvancedEntityAnimationNormalizedTimeByDeltaTimeAndAnimationSpeed) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = createEntity(false);

  const auto &animation =
      context.getComponent<liquid::AnimatorComponent>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(0.5f);
  EXPECT_EQ(animation.normalizedTime, 0.25f);
}

TEST_F(AnimationSystemTest, PausesEntityAnimationWhenItReachesAnimationEnd) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = createEntity(false);

  const auto &animation =
      context.getComponent<liquid::AnimatorComponent>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(1.5f);
  EXPECT_EQ(animation.normalizedTime, 1.0f);
  system.update(1.5f);
  EXPECT_EQ(animation.normalizedTime, 1.0f);
}

TEST_F(AnimationSystemTest, RestartsAnimationTimeIfLoop) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = createEntity(true);
  const auto &animation =
      context.getComponent<liquid::AnimatorComponent>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(1.0f);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest, UpdateEntityPositionBasedOnPositionKeyframe) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 1.0f);
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
  createAnimation(liquid::KeyframeSequenceAssetTarget::Rotation, 1.0f);
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
  createAnimation(liquid::KeyframeSequenceAssetTarget::Scale, 1.0f);
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

TEST_F(AnimationSystemTest,
       UpdateSkeletonPositionBasedOnPositionKeyframeWithJointTarget) {
  createSkeletonAnimation(liquid::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = createEntityWithSkeleton(false);

  const auto &transform =
      context.getComponent<liquid::TransformComponent>(entity);

  const auto &skeleton =
      context.getComponent<liquid::SkeletonComponent>(entity);
  EXPECT_EQ(skeleton.skeleton.getLocalPosition(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.skeleton.getLocalRotation(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.skeleton.getLocalScale(0), glm::vec3(1.0f));
  system.update(0.5f);
  EXPECT_EQ(skeleton.skeleton.getLocalPosition(0), glm::vec3(0.5f));
  EXPECT_EQ(skeleton.skeleton.getLocalRotation(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.skeleton.getLocalScale(0), glm::vec3(1.0f));

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest,
       UpdateSkeletonRotationBasedOnRotationKeyframeWithJointTarget) {
  createSkeletonAnimation(liquid::KeyframeSequenceAssetTarget::Rotation, 1.0f);
  auto entity = createEntityWithSkeleton(false);

  const auto &transform =
      context.getComponent<liquid::TransformComponent>(entity);

  const auto &skeleton =
      context.getComponent<liquid::SkeletonComponent>(entity);
  EXPECT_EQ(skeleton.skeleton.getLocalPosition(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.skeleton.getLocalRotation(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.skeleton.getLocalScale(0), glm::vec3(1.0f));
  system.update(0.5f);
  EXPECT_EQ(skeleton.skeleton.getLocalPosition(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.skeleton.getLocalRotation(0),
            glm::quat(0.5f, 0.5f, 0.5f, 0.5f));
  EXPECT_EQ(skeleton.skeleton.getLocalScale(0), glm::vec3(1.0f));

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest,
       UpdateSkeletonScaleBasedOnScaleKeyframeWithJointTarget) {
  createSkeletonAnimation(liquid::KeyframeSequenceAssetTarget::Scale, 1.0f);
  auto entity = createEntityWithSkeleton(false);

  const auto &transform =
      context.getComponent<liquid::TransformComponent>(entity);

  const auto &skeleton =
      context.getComponent<liquid::SkeletonComponent>(entity);
  EXPECT_EQ(skeleton.skeleton.getLocalPosition(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.skeleton.getLocalRotation(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.skeleton.getLocalScale(0), glm::vec3(1.0f));
  system.update(0.5f);
  EXPECT_EQ(skeleton.skeleton.getLocalPosition(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.skeleton.getLocalRotation(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.skeleton.getLocalScale(0), glm::vec3(0.5f));

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}
