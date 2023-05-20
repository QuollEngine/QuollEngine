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
      liquid::AnimationAssetHandle animIndex = liquid::AnimationAssetHandle{1},
      bool playing = true) {
    auto entity = entityDatabase.create();
    entityDatabase.set<liquid::LocalTransform>(entity, {});

    liquid::AssetData<liquid::AnimatorAsset> animatorAsset{};
    animatorAsset.data.initialState = 0;
    animatorAsset.data.states.push_back({"Animation", animIndex});

    auto animatorHandle = assetRegistry.getAnimators().addAsset(animatorAsset);

    liquid::Animator animator{};
    animator.playing = playing;
    animator.asset = animatorHandle;
    entityDatabase.set(entity, animator);

    return entity;
  }

  liquid::Entity createEntityWithSkeleton(
      liquid::AnimationAssetHandle animIndex = liquid::AnimationAssetHandle{1},
      bool playing = true) {
    auto entity = create(animIndex, playing);

    liquid::Skeleton skeleton{};
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

TEST_F(
    AnimationSystemTest,
    UpdateChangesAnimationStateIfCurrentStateCanTransitionUsingAnimationEvent) {
  auto animIndex =
      createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 2.0f);

  liquid::AnimationStateTransition transition0;
  transition0.eventName = "Move";
  transition0.target = 1;

  liquid::AnimationState state0{};
  state0.name = "Animation0";
  state0.animation = animIndex;
  state0.transitions.push_back(transition0);

  liquid::AnimationState state1{};
  state1.name = "Animation1";
  state1.animation = animIndex;

  liquid::AssetData<liquid::AnimatorAsset> animatorAsset{};
  animatorAsset.data.initialState = 0;
  animatorAsset.data.states.push_back(state0);
  animatorAsset.data.states.push_back(state1);

  auto animatorHandle = assetRegistry.getAnimators().addAsset(animatorAsset);

  auto entity = entityDatabase.create();

  {
    liquid::Animator animator{};
    animator.normalizedTime = 0.5f;
    animator.asset = animatorHandle;
    animator.currentState = 0;
    entityDatabase.set(entity, animator);
    entityDatabase.set<liquid::LocalTransform>(entity, {});
  }

  system.update(0.0f, entityDatabase);

  {
    const auto &animator = entityDatabase.get<liquid::Animator>(entity);
    EXPECT_EQ(animator.currentState, 0);
    EXPECT_EQ(animator.normalizedTime, 0.5f);
  }

  entityDatabase.set<liquid::AnimatorEvent>(entity, {"Move"});
  system.update(0.0f, entityDatabase);

  {
    const auto &animator = entityDatabase.get<liquid::Animator>(entity);
    EXPECT_EQ(animator.currentState, 1);
    EXPECT_EQ(animator.normalizedTime, 0.0f);
    EXPECT_FALSE(entityDatabase.has<liquid::AnimatorEvent>(entity));
  }
}

TEST_F(
    AnimationSystemTest,
    UpdateDoesNotChangeAnimationStateIfCurrentStateCannotTransitionUsingEvent) {
  auto animIndex =
      createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 2.0f);

  liquid::AnimationStateTransition transition0;
  transition0.eventName = "Move";
  transition0.target = 1;

  liquid::AnimationState state0{};
  state0.name = "Animation0";
  state0.animation = animIndex;
  state0.transitions.push_back(transition0);

  liquid::AnimationState state1{};
  state1.name = "Animation1";
  state1.animation = animIndex;

  liquid::AssetData<liquid::AnimatorAsset> animatorAsset{};
  animatorAsset.data.initialState = 0;
  animatorAsset.data.states.push_back(state0);
  animatorAsset.data.states.push_back(state1);

  auto animatorHandle = assetRegistry.getAnimators().addAsset(animatorAsset);

  auto entity = entityDatabase.create();

  {
    liquid::Animator animator{};
    animator.normalizedTime = 0.5f;
    animator.asset = animatorHandle;
    animator.currentState = 0;
    entityDatabase.set(entity, animator);
    entityDatabase.set<liquid::LocalTransform>(entity, {});
  }

  system.update(0.0f, entityDatabase);

  {
    const auto &animator = entityDatabase.get<liquid::Animator>(entity);
    EXPECT_EQ(animator.currentState, 0);
    EXPECT_EQ(animator.normalizedTime, 0.5f);
  }

  entityDatabase.set<liquid::AnimatorEvent>(entity, {"NotMove"});
  system.update(0.0f, entityDatabase);

  {
    const auto &animator = entityDatabase.get<liquid::Animator>(entity);
    EXPECT_EQ(animator.currentState, 0);
    EXPECT_EQ(animator.normalizedTime, 0.5f);
    EXPECT_FALSE(entityDatabase.has<liquid::AnimatorEvent>(entity));
  }
}

TEST_F(AnimationSystemTest, DoesNotAdvanceTimeIfComponentIsNotPlaying) {
  auto animIndex =
      createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create(animIndex, false);

  const auto &animation = entityDatabase.get<liquid::Animator>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(0.5f, entityDatabase);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest,
       AdvancedEntityAnimationNormalizedTimeByDeltaTimeAndAnimationSpeed) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create();

  const auto &animation = entityDatabase.get<liquid::Animator>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(0.5f, entityDatabase);
  EXPECT_EQ(animation.normalizedTime, 0.25f);
}

TEST_F(AnimationSystemTest, RestartsAnimationTimeWhenEndOfAnimationIsReached) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create();
  const auto &animation = entityDatabase.get<liquid::Animator>(entity);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
  system.update(1.0f, entityDatabase);
  EXPECT_EQ(animation.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest, UpdateEntityPositionBasedOnPositionKeyframe) {
  createAnimation(liquid::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create();

  const auto &transform = entityDatabase.get<liquid::LocalTransform>(entity);

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
  auto entity = create();

  const auto &transform = entityDatabase.get<liquid::LocalTransform>(entity);

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
  auto entity = create();

  const auto &transform = entityDatabase.get<liquid::LocalTransform>(entity);

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
  auto entity = createEntityWithSkeleton();

  const auto &transform = entityDatabase.get<liquid::LocalTransform>(entity);

  const auto &skeleton = entityDatabase.get<liquid::Skeleton>(entity);
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
  auto entity = createEntityWithSkeleton();

  const auto &transform = entityDatabase.get<liquid::LocalTransform>(entity);

  const auto &skeleton = entityDatabase.get<liquid::Skeleton>(entity);
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
  auto entity = createEntityWithSkeleton();

  const auto &transform = entityDatabase.get<liquid::LocalTransform>(entity);

  const auto &skeleton = entityDatabase.get<liquid::Skeleton>(entity);
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

TEST_F(AnimationSystemTest, DoesNothingIfAnimationDoesNotExist) {
  auto entity = create(liquid::AnimationAssetHandle::Invalid);
  system.update(0.5f, entityDatabase);
}
