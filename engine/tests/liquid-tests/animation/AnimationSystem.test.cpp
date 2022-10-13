#include "liquid/core/Base.h"
#include "liquid/animation/AnimationSystem.h"

#include "liquid-tests/Testing.h"

class AnimationSystemTest : public ::testing::Test {
public:
  liquid::EntityDatabase entityDatabase;
  liquid::AnimationSystem system;
  liquid::AssetRegistry assetRegistry;

  AnimationSystemTest() : system(assetRegistry) {}

  liquid::Entity create(
      bool loop,
      liquid::AnimationAssetHandle animIndex = liquid::AnimationAssetHandle{1},
      bool playing = true) {
    auto entity = entityDatabase.create();
    entityDatabase.set<liquid::LocalTransformComponent>(entity, {});
    entityDatabase.set<liquid::AnimatorComponent>(
        entity, {0, loop, 0.0f, playing, {animIndex}});

    return entity;
  }

  liquid::Entity createEntityWithSkeleton(
      bool loop,
      liquid::AnimationAssetHandle animIndex = liquid::AnimationAssetHandle{1},
      bool playing = true) {
    auto entity = create(loop, animIndex, playing);

    liquid::SkeletonComponent skeleton{};
    skeleton.jointFinalTransforms = {glm::mat4{1.0f}};
    skeleton.jointWorldTransforms = {glm::mat4{1.0f}};
    skeleton.jointLocalPositions = {glm::vec3{0.0f}};
    skeleton.jointLocalRotations = {glm::quat{1.0f, 0.0f, 0.0f, 0.0f}};
    skeleton.jointLocalScales = {glm::vec3{1.0f}};
    skeleton.jointNames = {"Joint0"};

    entityDatabase.set(entity, skeleton);

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
  auto entity = create(false, liquid::AnimationAssetHandle::Invalid);
  const auto &animation = entityDatabase.get<liquid::AnimatorComponent>(entity);

  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(0.5f, entityDatabase);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest, DoesNotAdvanceTimeIfComponentIsNotPlaying) {
  auto animIndex =
      createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create(false, animIndex, false);

  const auto &animation = entityDatabase.get<liquid::AnimatorComponent>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(0.5f, entityDatabase);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest,
       AdvancedEntityAnimationNormalizedTimeByDeltaTimeAndAnimationSpeed) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create(false);

  const auto &animation = entityDatabase.get<liquid::AnimatorComponent>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(0.5f, entityDatabase);
  EXPECT_EQ(animation.normalizedTime, 0.25f);
}

TEST_F(AnimationSystemTest, PausesEntityAnimationWhenItReachesAnimationEnd) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create(false);

  const auto &animation = entityDatabase.get<liquid::AnimatorComponent>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(1.5f, entityDatabase);
  EXPECT_EQ(animation.normalizedTime, 1.0f);
  system.update(1.5f, entityDatabase);
  EXPECT_EQ(animation.normalizedTime, 1.0f);
}

TEST_F(AnimationSystemTest, RestartsAnimationTimeIfLoop) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create(true);
  const auto &animation = entityDatabase.get<liquid::AnimatorComponent>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(1.0f, entityDatabase);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest, UpdateEntityPositionBasedOnPositionKeyframe) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create(false);

  const auto &transform =
      entityDatabase.get<liquid::LocalTransformComponent>(entity);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
  system.update(0.5f, entityDatabase);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.5f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest, UpdateEntityRotationBasedOnRotationKeyframe) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Rotation, 1.0f);
  auto entity = create(false);

  const auto &transform =
      entityDatabase.get<liquid::LocalTransformComponent>(entity);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
  system.update(0.5f, entityDatabase);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(0.5f, 0.5f, 0.5f, 0.5f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest, UpdateEntityScaleBasedOnScaleKeyframe) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Scale, 1.0f);
  auto entity = create(false);

  const auto &transform =
      entityDatabase.get<liquid::LocalTransformComponent>(entity);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
  system.update(0.5f, entityDatabase);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(0.5f));
}

TEST_F(AnimationSystemTest,
       UpdateSkeletonPositionBasedOnPositionKeyframeWithJointTarget) {
  createSkeletonAnimation(liquid::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = createEntityWithSkeleton(false);

  const auto &transform =
      entityDatabase.get<liquid::LocalTransformComponent>(entity);

  const auto &skeleton = entityDatabase.get<liquid::SkeletonComponent>(entity);
  EXPECT_EQ(skeleton.jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.jointLocalScales.at(0), glm::vec3(1.0f));
  system.update(0.5f, entityDatabase);
  EXPECT_EQ(skeleton.jointLocalPositions.at(0), glm::vec3(0.5f));
  EXPECT_EQ(skeleton.jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.jointLocalScales.at(0), glm::vec3(1.0f));

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest,
       UpdateSkeletonRotationBasedOnRotationKeyframeWithJointTarget) {
  createSkeletonAnimation(liquid::KeyframeSequenceAssetTarget::Rotation, 1.0f);
  auto entity = createEntityWithSkeleton(false);

  const auto &transform =
      entityDatabase.get<liquid::LocalTransformComponent>(entity);

  const auto &skeleton = entityDatabase.get<liquid::SkeletonComponent>(entity);
  EXPECT_EQ(skeleton.jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.jointLocalScales.at(0), glm::vec3(1.0f));
  system.update(0.5f, entityDatabase);
  EXPECT_EQ(skeleton.jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.jointLocalRotations.at(0),
            glm::quat(0.5f, 0.5f, 0.5f, 0.5f));
  EXPECT_EQ(skeleton.jointLocalScales.at(0), glm::vec3(1.0f));

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest,
       UpdateSkeletonScaleBasedOnScaleKeyframeWithJointTarget) {
  createSkeletonAnimation(liquid::KeyframeSequenceAssetTarget::Scale, 1.0f);
  auto entity = createEntityWithSkeleton(false);

  const auto &transform =
      entityDatabase.get<liquid::LocalTransformComponent>(entity);

  const auto &skeleton = entityDatabase.get<liquid::SkeletonComponent>(entity);
  EXPECT_EQ(skeleton.jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.jointLocalScales.at(0), glm::vec3(1.0f));
  system.update(0.5f, entityDatabase);
  EXPECT_EQ(skeleton.jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.jointLocalScales.at(0), glm::vec3(0.5f));

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}
