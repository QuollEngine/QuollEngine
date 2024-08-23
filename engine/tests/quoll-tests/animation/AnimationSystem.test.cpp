#include "quoll/core/Base.h"
#include "quoll/animation/AnimationSystem.h"
#include "quoll/animation/Animator.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll/asset/AssetRegistry.h"
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
  quoll::AnimationSystem system;
  quoll::AssetRegistry assetRegistry;
  quoll::SystemView view{&scene};

  AnimationSystemTest() : system(assetRegistry) {
    system.createSystemViewData(view);
  }

  quoll::Entity
  create(quoll::AnimationAssetHandle animIndex = quoll::AnimationAssetHandle{1},
         bool playing = true, f32 speed = 1.0f,
         quoll::AnimationLoopMode loopMode = quoll::AnimationLoopMode::None) {
    auto entity = entityDatabase.entity();
    entity.set<quoll::LocalTransform>({});

    quoll::AssetData<quoll::AnimatorAsset> animatorAsset{};
    animatorAsset.data.initialState = 0;
    animatorAsset.data.states.push_back({.name = "Animation",
                                         .animation = animIndex,
                                         .speed = speed,
                                         .loopMode = loopMode});

    auto animatorHandle = assetRegistry.getAnimators().addAsset(animatorAsset);

    quoll::Animator animator{};
    animator.playing = playing;
    animator.asset = animatorHandle;
    entity.set(animator);

    return entity;
  }

  quoll::Entity createEntityWithSkeleton(
      quoll::AnimationAssetHandle animIndex = quoll::AnimationAssetHandle{1},
      bool playing = true) {
    auto entity = create(animIndex, playing);

    quoll::Skeleton skeleton{};
    skeleton.jointFinalTransforms = {glm::mat4{1.0f}};
    skeleton.jointWorldTransforms = {glm::mat4{1.0f}};
    skeleton.jointLocalPositions = {glm::vec3{0.0f}};
    skeleton.jointLocalRotations = {glm::quat{1.0f, 0.0f, 0.0f, 0.0f}};
    skeleton.jointLocalScales = {glm::vec3{1.0f}};
    skeleton.jointNames = {"Joint0"};

    entity.set(skeleton);

    return entity;
  }

  quoll::AnimationAssetHandle
  createAnimation(quoll::KeyframeSequenceAssetTarget target, f32 time) {
    quoll::AssetData<quoll::AnimationAsset> animation;
    animation.data.time = time;

    quoll::KeyframeSequenceAsset sequence;
    sequence.target = target;
    sequence.interpolation = quoll::KeyframeSequenceAssetInterpolation::Step;

    sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
    sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f),
                               glm::vec4(1.0f)};

    animation.data.keyframes.push_back(sequence);

    return assetRegistry.getAnimations().addAsset(animation);
  }

  quoll::AnimationAssetHandle
  createSkeletonAnimation(quoll::KeyframeSequenceAssetTarget target, f32 time) {
    quoll::AssetData<quoll::AnimationAsset> animation;
    animation.data.time = time;

    quoll::KeyframeSequenceAsset sequence;
    sequence.target = target;
    sequence.interpolation = quoll::KeyframeSequenceAssetInterpolation::Step;
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
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);

  quoll::AnimationStateTransition transition0;
  transition0.eventName = "Move";
  transition0.target = 1;

  quoll::AnimationState state0{};
  state0.name = "Animation0";
  state0.animation = animIndex;
  state0.transitions.push_back(transition0);

  quoll::AnimationState state1{};
  state1.name = "Animation1";
  state1.animation = animIndex;

  quoll::AssetData<quoll::AnimatorAsset> animatorAsset{};
  animatorAsset.data.initialState = 0;
  animatorAsset.data.states.push_back(state0);
  animatorAsset.data.states.push_back(state1);

  auto animatorHandle = assetRegistry.getAnimators().addAsset(animatorAsset);

  auto entity = entityDatabase.entity();

  {
    quoll::Animator animator{};
    animator.normalizedTime = 0.5f;
    animator.asset = animatorHandle;
    animator.currentState = 0;
    entity.set(animator);
    entity.set<quoll::LocalTransform>({});
  }

  system.prepare(view);
  system.update(0.0f, view);

  {
    auto animator = entity.get_ref<quoll::Animator>();
    EXPECT_EQ(animator->currentState, 0);
    EXPECT_EQ(animator->normalizedTime, 0.5f);
  }

  entity.set<quoll::AnimatorEvent>({"Move"});
  system.prepare(view);
  system.update(0.0f, view);

  {
    auto animator = entity.get_ref<quoll::Animator>();
    EXPECT_EQ(animator->currentState, 1);
    EXPECT_EQ(animator->normalizedTime, 0.0f);
    EXPECT_FALSE(entity.has<quoll::AnimatorEvent>());
  }
}

TEST_F(
    AnimationSystemTest,
    UpdateDoesNotChangeAnimationStateIfCurrentStateCannotTransitionUsingEvent) {
  auto animIndex =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);

  quoll::AnimationStateTransition transition0;
  transition0.eventName = "Move";
  transition0.target = 1;

  quoll::AnimationState state0{};
  state0.name = "Animation0";
  state0.animation = animIndex;
  state0.transitions.push_back(transition0);

  quoll::AnimationState state1{};
  state1.name = "Animation1";
  state1.animation = animIndex;

  quoll::AssetData<quoll::AnimatorAsset> animatorAsset{};
  animatorAsset.data.initialState = 0;
  animatorAsset.data.states.push_back(state0);
  animatorAsset.data.states.push_back(state1);

  auto animatorHandle = assetRegistry.getAnimators().addAsset(animatorAsset);

  auto entity = entityDatabase.entity();

  {
    quoll::Animator animator{};
    animator.normalizedTime = 0.5f;
    animator.asset = animatorHandle;
    animator.currentState = 0;
    entity.set(animator);
    entity.set<quoll::LocalTransform>({});
  }

  system.prepare(view);
  system.update(0.0f, view);

  {
    auto animator = entity.get_ref<quoll::Animator>();
    EXPECT_EQ(animator->currentState, 0);
    EXPECT_EQ(animator->normalizedTime, 0.5f);
  }

  entity.set<quoll::AnimatorEvent>({"NotMove"});

  system.prepare(view);
  system.update(0.0f, view);

  {
    auto animator = entity.get_ref<quoll::Animator>();
    EXPECT_EQ(animator->currentState, 0);
    EXPECT_EQ(animator->normalizedTime, 0.5f);
    EXPECT_FALSE(entity.has<quoll::AnimatorEvent>());
  }
}

TEST_F(AnimationSystemTest, DoesNotAdvanceTimeIfComponentIsNotPlaying) {
  auto animIndex =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create(animIndex, false);

  auto animator = entity.get_ref<quoll::Animator>();
  EXPECT_EQ(animator->normalizedTime, 0.0f);
  system.prepare(view);
  system.update(0.5f, view);
  EXPECT_EQ(animator->normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest, AdvancesAnimatorNormalizedTimeByDeltaTime) {
  createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create();

  auto animator = entity.get_ref<quoll::Animator>();
  EXPECT_EQ(animator->normalizedTime, 0.0f);
  system.prepare(view);
  system.update(0.5f, view);
  EXPECT_EQ(animator->normalizedTime, 0.25f);
}

TEST_F(AnimationSystemTest,
       AdvancesAnimatorNormalizedTimeByDeltaTimeAndAnimationStateSpeed) {
  auto anim0 =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create(anim0, true, 0.5f);

  auto animator = entity.get_ref<quoll::Animator>();
  EXPECT_EQ(animator->normalizedTime, 0.0f);
  system.prepare(view);
  system.update(0.5f, view);
  EXPECT_EQ(animator->normalizedTime, 0.125f);
}

TEST_F(AnimationSystemTest,
       StaysAtTheEndIfEndOfAnimationIsReachedAndLoopModeIsNone) {
  createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create();
  auto animator = entity.get_ref<quoll::Animator>();
  EXPECT_EQ(animator->normalizedTime, 0.0f);
  system.prepare(view);
  system.update(1.0f, view);
  EXPECT_EQ(animator->normalizedTime, 1.0f);
}

TEST_F(AnimationSystemTest,
       RestartsAnimationTimeWhenEndOfAnimationIsReachedAndLoopModeIsLinear) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create(animation, true, 1.0f, quoll::AnimationLoopMode::Linear);
  auto animator = entity.get_ref<quoll::Animator>();
  EXPECT_EQ(animator->normalizedTime, 0.0f);
  system.prepare(view);
  system.update(1.0f, view);
  EXPECT_EQ(animator->normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest, UpdateEntityPositionBasedOnPositionKeyframe) {
  createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create();

  auto transform = entity.get_ref<quoll::LocalTransform>();

  EXPECT_EQ(transform->localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform->localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform->localScale, glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);

  EXPECT_EQ(transform->localPosition, glm::vec3(0.5f));
  EXPECT_EQ(transform->localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform->localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest, UpdateEntityRotationBasedOnRotationKeyframe) {
  createAnimation(quoll::KeyframeSequenceAssetTarget::Rotation, 1.0f);
  auto entity = create();

  auto transform = entity.get_ref<quoll::LocalTransform>();

  EXPECT_EQ(transform->localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform->localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform->localScale, glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);

  EXPECT_EQ(transform->localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform->localRotation, glm::quat(0.5f, 0.5f, 0.5f, 0.5f));
  EXPECT_EQ(transform->localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest, UpdateEntityScaleBasedOnScaleKeyframe) {
  createAnimation(quoll::KeyframeSequenceAssetTarget::Scale, 1.0f);
  auto entity = create();

  auto transform = entity.get_ref<quoll::LocalTransform>();

  EXPECT_EQ(transform->localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform->localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform->localScale, glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);

  EXPECT_EQ(transform->localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform->localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform->localScale, glm::vec3(0.5f));
}

TEST_F(AnimationSystemTest,
       UpdateSkeletonPositionBasedOnPositionKeyframeWithJointTarget) {
  createSkeletonAnimation(quoll::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = createEntityWithSkeleton();

  auto transform = entity.get_ref<quoll::LocalTransform>();

  auto skeleton = entity.get_ref<quoll::Skeleton>();
  EXPECT_EQ(skeleton->jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton->jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton->jointLocalScales.at(0), glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);
  EXPECT_EQ(skeleton->jointLocalPositions.at(0), glm::vec3(0.5f));
  EXPECT_EQ(skeleton->jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton->jointLocalScales.at(0), glm::vec3(1.0f));

  EXPECT_EQ(transform->localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform->localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform->localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest,
       UpdateSkeletonRotationBasedOnRotationKeyframeWithJointTarget) {
  createSkeletonAnimation(quoll::KeyframeSequenceAssetTarget::Rotation, 1.0f);
  auto entity = createEntityWithSkeleton();

  auto transform = entity.get_ref<quoll::LocalTransform>();

  auto skeleton = entity.get_ref<quoll::Skeleton>();
  EXPECT_EQ(skeleton->jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton->jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton->jointLocalScales.at(0), glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);
  EXPECT_EQ(skeleton->jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton->jointLocalRotations.at(0),
            glm::quat(0.5f, 0.5f, 0.5f, 0.5f));
  EXPECT_EQ(skeleton->jointLocalScales.at(0), glm::vec3(1.0f));

  EXPECT_EQ(transform->localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform->localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform->localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest,
       UpdateSkeletonScaleBasedOnScaleKeyframeWithJointTarget) {
  createSkeletonAnimation(quoll::KeyframeSequenceAssetTarget::Scale, 1.0f);
  auto entity = createEntityWithSkeleton();

  auto transform = entity.get_ref<quoll::LocalTransform>();

  auto skeleton = entity.get_ref<quoll::Skeleton>();
  EXPECT_EQ(skeleton->jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton->jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton->jointLocalScales.at(0), glm::vec3(1.0f));
  system.prepare(view);
  system.update(0.5f, view);
  EXPECT_EQ(skeleton->jointLocalPositions.at(0), glm::vec3(0.0f));
  EXPECT_EQ(skeleton->jointLocalRotations.at(0),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(skeleton->jointLocalScales.at(0), glm::vec3(0.5f));

  EXPECT_EQ(transform->localPosition, glm::vec3(0.0f));
  EXPECT_EQ(transform->localRotation, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(transform->localScale, glm::vec3(1.0f));
}

TEST_F(AnimationSystemTest, DoesNothingIfAnimationDoesNotExist) {
  auto entity = create(quoll::AnimationAssetHandle::Null);
  system.prepare(view);
  system.update(0.5f, view);
}
