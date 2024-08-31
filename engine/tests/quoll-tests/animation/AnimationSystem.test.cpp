#include "quoll/core/Base.h"
#include "quoll/animation/AnimationAsset.h"
#include "quoll/animation/AnimationSystem.h"
#include "quoll/animation/Animator.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Scene.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/system/SystemView.h"
#include "quoll-tests/Testing.h"

class AnimationSystemTest : public ::testing::Test {
public:
  quoll::Scene scene;
  quoll::EntityDatabase &entityDatabase = scene.entityDatabase;
  quoll::AssetCache assetCache;
  quoll::AnimationSystem system;

  quoll::SystemView view{&scene};

  AnimationSystemTest() : assetCache("/"), system(assetCache.getRegistry()) {}

  quoll::Entity
  create(quoll::AssetRef<quoll::AnimationAsset> animation, bool playing = true,
         f32 speed = 1.0f,
         quoll::AnimationLoopMode loopMode = quoll::AnimationLoopMode::None) {
    auto entity = entityDatabase.create();
    entityDatabase.set<quoll::LocalTransform>(entity, {});

    quoll::AssetData<quoll::AnimatorAsset> animatorAsset{};
    animatorAsset.uuid = quoll::Uuid::generate();
    animatorAsset.data.initialState = 0;
    animatorAsset.data.states.push_back({.name = "Animation",
                                         .animation = animation,
                                         .speed = speed,
                                         .loopMode = loopMode});

    auto animatorHandle = assetCache.getRegistry().add(animatorAsset);

    quoll::Animator animator{};
    animator.playing = playing;
    animator.asset = animatorHandle;
    entityDatabase.set(entity, animator);

    return entity;
  }

  quoll::Entity
  createEntityWithSkeleton(quoll::AssetRef<quoll::AnimationAsset> animation,
                           bool playing = true) {
    auto entity = create(animation, playing);

    quoll::Skeleton skeleton{};
    skeleton.jointFinalTransforms = {glm::mat4{1.0f}};
    skeleton.jointWorldTransforms = {glm::mat4{1.0f}};
    skeleton.jointLocalPositions = {glm::vec3{0.0f}};
    skeleton.jointLocalRotations = {glm::quat{1.0f, 0.0f, 0.0f, 0.0f}};
    skeleton.jointLocalScales = {glm::vec3{1.0f}};
    skeleton.jointNames = {"Joint0"};

    entityDatabase.set(entity, skeleton);

    return entity;
  }

  quoll::AssetRef<quoll::AnimationAsset>
  createAnimation(quoll::KeyframeSequenceAssetTarget target, f32 time) {
    quoll::AssetData<quoll::AnimationAsset> animation;
    animation.uuid = quoll::Uuid::generate();
    animation.data.time = time;

    quoll::KeyframeSequenceAsset sequence;
    sequence.target = target;
    sequence.interpolation = quoll::KeyframeSequenceAssetInterpolation::Step;

    sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
    sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f),
                               glm::vec4(1.0f)};

    animation.data.keyframes.push_back(sequence);

    assetCache.getRegistry().add(animation);

    return assetCache.request<quoll::AnimationAsset>(animation.uuid);
  }

  quoll::AssetRef<quoll::AnimationAsset>
  createSkeletonAnimation(quoll::KeyframeSequenceAssetTarget target, f32 time) {
    quoll::AssetData<quoll::AnimationAsset> animation;
    animation.data.time = time;
    animation.uuid = quoll::Uuid::generate();

    quoll::KeyframeSequenceAsset sequence;
    sequence.target = target;
    sequence.interpolation = quoll::KeyframeSequenceAssetInterpolation::Step;
    sequence.joint = 0;
    sequence.jointTarget = true;

    sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
    sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f),
                               glm::vec4(1.0f)};

    animation.data.keyframes.push_back(sequence);

    assetCache.getRegistry().add(animation);
    return assetCache.request<quoll::AnimationAsset>(animation.uuid);
  }
};

using AnimationSystemDeathTest = AnimationSystemTest;

TEST_F(
    AnimationSystemTest,
    UpdateChangesAnimationStateIfCurrentStateCanTransitionUsingAnimationEvent) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);

  quoll::AnimationStateTransition transition0;
  transition0.eventName = "Move";
  transition0.target = 1;

  quoll::AnimationState state0{};
  state0.name = "Animation0";
  state0.animation = animation;
  state0.transitions.push_back(transition0);

  quoll::AnimationState state1{};
  state1.name = "Animation1";
  state1.animation = animation;

  quoll::AssetData<quoll::AnimatorAsset> animatorAsset{};
  animatorAsset.data.initialState = 0;
  animatorAsset.data.states.push_back(state0);
  animatorAsset.data.states.push_back(state1);

  auto animatorHandle = assetCache.getRegistry().add(animatorAsset);

  auto entity = entityDatabase.create();

  {
    quoll::Animator animator{};
    animator.normalizedTime = 0.5f;
    animator.asset = animatorHandle;
    animator.currentState = 0;
    entityDatabase.set(entity, animator);
    entityDatabase.set<quoll::LocalTransform>(entity, {});
  }

  system.prepare(view);
  system.update(0.0f, view);

  {
    const auto &animator = entityDatabase.get<quoll::Animator>(entity);
    EXPECT_EQ(animator.currentState, 0);
    EXPECT_EQ(animator.normalizedTime, 0.5f);
  }

  entityDatabase.set<quoll::AnimatorEvent>(entity, {"Move"});
  system.prepare(view);
  system.update(0.0f, view);

  {
    const auto &animator = entityDatabase.get<quoll::Animator>(entity);
    EXPECT_EQ(animator.currentState, 1);
    EXPECT_EQ(animator.normalizedTime, 0.0f);
    EXPECT_FALSE(entityDatabase.has<quoll::AnimatorEvent>(entity));
  }
}

TEST_F(
    AnimationSystemTest,
    UpdateDoesNotChangeAnimationStateIfCurrentStateCannotTransitionUsingEvent) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);

  quoll::AnimationStateTransition transition0;
  transition0.eventName = "Move";
  transition0.target = 1;

  quoll::AnimationState state0{};
  state0.name = "Animation0";
  state0.animation = animation;
  state0.transitions.push_back(transition0);

  quoll::AnimationState state1{};
  state1.name = "Animation1";
  state1.animation = animation;

  quoll::AssetData<quoll::AnimatorAsset> animatorAsset{};
  animatorAsset.data.initialState = 0;
  animatorAsset.data.states.push_back(state0);
  animatorAsset.data.states.push_back(state1);

  auto animatorHandle = assetCache.getRegistry().add(animatorAsset);

  auto entity = entityDatabase.create();

  {
    quoll::Animator animator{};
    animator.normalizedTime = 0.5f;
    animator.asset = animatorHandle;
    animator.currentState = 0;
    entityDatabase.set(entity, animator);
    entityDatabase.set<quoll::LocalTransform>(entity, {});
  }

  system.prepare(view);
  system.update(0.0f, view);

  {
    const auto &animator = entityDatabase.get<quoll::Animator>(entity);
    EXPECT_EQ(animator.currentState, 0);
    EXPECT_EQ(animator.normalizedTime, 0.5f);
  }

  entityDatabase.set<quoll::AnimatorEvent>(entity, {"NotMove"});

  system.prepare(view);
  system.update(0.0f, view);

  {
    const auto &animator = entityDatabase.get<quoll::Animator>(entity);
    EXPECT_EQ(animator.currentState, 0);
    EXPECT_EQ(animator.normalizedTime, 0.5f);
    EXPECT_FALSE(entityDatabase.has<quoll::AnimatorEvent>(entity));
  }
}

TEST_F(AnimationSystemTest, DoesNotAdvanceTimeIfComponentIsNotPlaying) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create(animation, false);

  const auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.normalizedTime, 0.0f);
  system.prepare(view);
  system.update(0.5f, view);
  EXPECT_EQ(animator.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest, AdvancesAnimatorNormalizedTimeByDeltaTime) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create(animation);

  const auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.normalizedTime, 0.0f);
  system.prepare(view);
  system.update(0.5f, view);
  EXPECT_EQ(animator.normalizedTime, 0.25f);
}

TEST_F(AnimationSystemTest,
       AdvancesAnimatorNormalizedTimeByDeltaTimeAndAnimationStateSpeed) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create(animation, true, 0.5f);

  const auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.normalizedTime, 0.0f);
  system.prepare(view);
  system.update(0.5f, view);
  EXPECT_EQ(animator.normalizedTime, 0.125f);
}

TEST_F(AnimationSystemTest,
       StaysAtTheEndIfEndOfAnimationIsReachedAndLoopModeIsNone) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create(animation);
  const auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.normalizedTime, 0.0f);
  system.prepare(view);
  system.update(1.0f, view);
  EXPECT_EQ(animator.normalizedTime, 1.0f);
}

TEST_F(AnimationSystemTest,
       RestartsAnimationTimeWhenEndOfAnimationIsReachedAndLoopModeIsLinear) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create(animation, true, 1.0f, quoll::AnimationLoopMode::Linear);
  const auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.normalizedTime, 0.0f);
  system.prepare(view);
  system.update(1.0f, view);
  EXPECT_EQ(animator.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest, UpdateEntityPositionBasedOnPositionKeyframe) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create(animation);

  const auto &transform = entityDatabase.get<quoll::LocalTransform>(entity);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.5f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest, UpdateEntityRotationBasedOnRotationKeyframe) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Rotation, 1.0f);
  auto entity = create(animation);

  const auto &transform = entityDatabase.get<quoll::LocalTransform>(entity);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(0.5f, 0.5f, 0.5f, 0.5f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest, UpdateEntityScaleBasedOnScaleKeyframe) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Scale, 1.0f);
  auto entity = create(animation);

  const auto &transform = entityDatabase.get<quoll::LocalTransform>(entity);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(0.5f));
}

TEST_F(AnimationSystemTest,
       UpdateSkeletonPositionBasedOnPositionKeyframeWithJointTarget) {
  auto animation = createSkeletonAnimation(
      quoll::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = createEntityWithSkeleton(animation);

  const auto &transform = entityDatabase.get<quoll::LocalTransform>(entity);

  const auto &skeleton = entityDatabase.get<quoll::Skeleton>(entity);
  EXPECT_EQ(skeleton.jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.jointLocalScales.at(0), glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);
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
  auto animation = createSkeletonAnimation(
      quoll::KeyframeSequenceAssetTarget::Rotation, 1.0f);
  auto entity = createEntityWithSkeleton(animation);

  const auto &transform = entityDatabase.get<quoll::LocalTransform>(entity);

  const auto &skeleton = entityDatabase.get<quoll::Skeleton>(entity);
  EXPECT_EQ(skeleton.jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.jointLocalScales.at(0), glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);
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
  auto animation =
      createSkeletonAnimation(quoll::KeyframeSequenceAssetTarget::Scale, 1.0f);
  auto entity = createEntityWithSkeleton(animation);

  const auto &transform = entityDatabase.get<quoll::LocalTransform>(entity);

  const auto &skeleton = entityDatabase.get<quoll::Skeleton>(entity);
  EXPECT_EQ(skeleton.jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.jointLocalScales.at(0), glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);
  EXPECT_EQ(skeleton.jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton.jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton.jointLocalScales.at(0), glm::vec3(0.5f));

  EXPECT_EQ(transform.localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform.localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform.localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest, DoesNothingIfAnimationDoesNotExist) {
  auto entity = create(quoll::AssetRef<quoll::AnimationAsset>());
  system.prepare(view);
  system.update(0.5f, view);
}
