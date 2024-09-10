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
#include "quoll-tests/test-utils/AssetCacheUtils.h"

class AnimationSystemTest : public ::testing::Test {
public:
  quoll::AssetCache assetCache;
  quoll::Scene scene;
  quoll::EntityDatabase &entityDatabase = scene.entityDatabase;
  quoll::AnimationSystem system{};

  quoll::SystemView view{&scene};

  AnimationSystemTest() : assetCache("/") {}

  template <typename TAssetData>
  quoll::AssetRef<TAssetData> createAsset(TAssetData data = {}) {
    return createAssetInCache(assetCache, data);
  }

  quoll::Entity
  create(quoll::AssetRef<quoll::AnimationAsset> animation, bool playing = true,
         f32 speed = 1.0f,
         quoll::AnimationLoopMode loopMode = quoll::AnimationLoopMode::None) {
    auto entity = entityDatabase.create();
    entityDatabase.set<quoll::LocalTransform>(entity, {});

    std::vector states{quoll::AnimationState{.name = "Animation",
                                             .animation = animation,
                                             .speed = speed,
                                             .loopMode = loopMode}};

    auto asset = createAsset<quoll::AnimatorAsset>({.states = states});

    quoll::AnimatorAssetRef animatorRef{};
    animatorRef.asset = asset;
    entityDatabase.set(entity, animatorRef);

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
    quoll::AnimationAsset animation;
    animation.time = time;

    quoll::KeyframeSequenceAsset sequence;
    sequence.target = target;
    sequence.interpolation = quoll::KeyframeSequenceAssetInterpolation::Step;

    sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
    sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f),
                               glm::vec4(1.0f)};

    animation.keyframes.push_back(sequence);

    return createAsset(animation);
  }

  quoll::AssetRef<quoll::AnimationAsset>
  createSkeletonAnimation(quoll::KeyframeSequenceAssetTarget target, f32 time) {
    quoll::AnimationAsset animation;
    animation.time = time;

    quoll::KeyframeSequenceAsset sequence;
    sequence.target = target;
    sequence.interpolation = quoll::KeyframeSequenceAssetInterpolation::Step;
    sequence.joint = 0;
    sequence.jointTarget = true;

    sequence.keyframeTimes = {0.0f, 0.5f, 1.0f};
    sequence.keyframeValues = {glm::vec4(0.0f), glm::vec4(0.5f),
                               glm::vec4(1.0f)};

    animation.keyframes.push_back(sequence);

    return createAsset(animation);
  }
};

using AnimationSystemDeathTest = AnimationSystemTest;

TEST_F(AnimationSystemTest, PrepareDoesNotCreateAnimatorIfNoRefComponent) {
  auto entity = entityDatabase.create();

  system.prepare(view);

  EXPECT_FALSE(entityDatabase.has<quoll::Animator>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::AnimatorAssetRef>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::AnimatorCurrentAsset>(entity));
}

TEST_F(AnimationSystemTest, PrepareDoesNotCreateAnimatorIfAssetHasNoData) {
  auto ref = createAssetInCacheWithoutData<quoll::AnimatorAsset>(assetCache);

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AnimatorAssetRef>(entity, {ref});

  system.prepare(view);

  EXPECT_FALSE(entityDatabase.has<quoll::Animator>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::AnimatorCurrentAsset>(entity));
}

TEST_F(AnimationSystemTest,
       PrepareDoesNotCreateAnimatorIfAssetAnimationDoesNotHaveData) {
  auto anim0 = createAssetInCacheWithoutData<quoll::AnimationAsset>(assetCache);
  auto anim1 = createAssetInCache<quoll::AnimationAsset>(assetCache);

  auto states = {
      quoll::AnimationState{.animation = anim0},
      quoll::AnimationState{.animation = anim1},
  };

  auto ref =
      createAssetInCache<quoll::AnimatorAsset>(assetCache, {.states = states});

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AnimatorAssetRef>(entity, {ref});

  system.prepare(view);

  EXPECT_FALSE(entityDatabase.has<quoll::Animator>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::AnimatorCurrentAsset>(entity));
}

TEST_F(AnimationSystemTest,
       PrepareCreatesAnimatorIfAnimatorAssetAndItsAnimationsAreFullyLoaded) {
  auto anim0 = createAssetInCache<quoll::AnimationAsset>(assetCache);
  auto anim1 = createAssetInCache<quoll::AnimationAsset>(assetCache);

  auto states = {
      quoll::AnimationState{.animation = anim0},
      quoll::AnimationState{.animation = anim1},
  };

  auto ref = createAssetInCache<quoll::AnimatorAsset>(
      assetCache, {.initialState = 1, .states = states});

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AnimatorAssetRef>(entity, {ref});

  system.prepare(view);

  ASSERT_TRUE(entityDatabase.has<quoll::Animator>(entity));
  ASSERT_TRUE(entityDatabase.has<quoll::AnimatorCurrentAsset>(entity));

  auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.asset, ref);
  EXPECT_EQ(animator.currentState, 1);
  EXPECT_EQ(animator.normalizedTime, 0.0f);
  EXPECT_EQ(animator.playing, true);

  EXPECT_EQ(entityDatabase.get<quoll::AnimatorCurrentAsset>(entity).handle,
            ref.handle());
}

TEST_F(AnimationSystemTest, PrepareDoesNotUpdateAnimatorIfAssetHasNotChanged) {
  auto anim0 = createAssetInCache<quoll::AnimationAsset>(assetCache);
  auto anim1 = createAssetInCache<quoll::AnimationAsset>(assetCache);

  auto states = {
      quoll::AnimationState{.animation = anim0},
      quoll::AnimationState{.animation = anim1},
  };

  auto ref =
      createAssetInCache<quoll::AnimatorAsset>(assetCache, {.states = states});

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AnimatorAssetRef>(entity, {ref});

  system.prepare(view);

  {
    auto &animator = entityDatabase.get<quoll::Animator>(entity);
    animator.currentState = 1;
    animator.normalizedTime = 0.5f;
    animator.playing = false;
  }

  entityDatabase.set<quoll::AnimatorAssetRef>(entity, {ref});

  system.prepare(view);

  ASSERT_TRUE(entityDatabase.has<quoll::Animator>(entity));
  ASSERT_TRUE(entityDatabase.has<quoll::AnimatorCurrentAsset>(entity));

  auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.asset, ref);
  EXPECT_EQ(animator.currentState, 1);
  EXPECT_EQ(animator.normalizedTime, 0.5f);
  EXPECT_EQ(animator.playing, false);

  EXPECT_EQ(entityDatabase.get<quoll::AnimatorCurrentAsset>(entity).handle,
            ref.handle());
}

TEST_F(AnimationSystemTest, PrepareUpdatesAnimatorIfAssetHasChanged) {
  auto anim0 = createAssetInCache<quoll::AnimationAsset>(assetCache);
  auto anim1 = createAssetInCache<quoll::AnimationAsset>(assetCache);

  auto states = {
      quoll::AnimationState{.animation = anim0},
      quoll::AnimationState{.animation = anim1},
  };

  auto ref1 =
      createAssetInCache<quoll::AnimatorAsset>(assetCache, {.states = states});

  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AnimatorAssetRef>(entity, {ref1});

  system.prepare(view);

  {
    auto &animator = entityDatabase.get<quoll::Animator>(entity);
    animator.currentState = 1;
    animator.normalizedTime = 0.5f;
    animator.playing = false;
  }

  // Note: It does not matter if animator asset has the same states
  // as long as it is a new asset, it will be treated as a new one
  auto ref2 =
      createAssetInCache<quoll::AnimatorAsset>(assetCache, {.states = states});
  entityDatabase.set<quoll::AnimatorAssetRef>(entity, {ref2});

  system.prepare(view);

  ASSERT_TRUE(entityDatabase.has<quoll::Animator>(entity));
  ASSERT_TRUE(entityDatabase.has<quoll::AnimatorCurrentAsset>(entity));

  auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.asset, ref2);
  EXPECT_EQ(animator.currentState, 0);
  EXPECT_EQ(animator.normalizedTime, 0.0f);
  EXPECT_EQ(animator.playing, true);

  EXPECT_EQ(entityDatabase.get<quoll::AnimatorCurrentAsset>(entity).handle,
            ref2.handle());
}

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

  auto animatorAsset = createAsset<quoll::AnimatorAsset>(
      {.initialState = 0, .states = {state0, state1}});

  auto entity = entityDatabase.create();

  {
    quoll::AnimatorAssetRef animator{};
    animator.asset = animatorAsset;
    entityDatabase.set(entity, animator);
    entityDatabase.set<quoll::LocalTransform>(entity, {});
  }

  system.prepare(view);
  system.update(1.0f, view);

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

  auto animatorAsset = createAsset<quoll::AnimatorAsset>(
      {.initialState = 0, .states = {state0, state1}});

  auto entity = entityDatabase.create();

  {
    quoll::Animator animator{};
    animator.normalizedTime = 0.5f;
    animator.asset = animatorAsset;
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

  system.prepare(view);
  auto &animator = entityDatabase.get<quoll::Animator>(entity);
  animator.playing = false;

  system.update(0.5f, view);
  EXPECT_EQ(animator.normalizedTime, 0.0f);
}

TEST_F(AnimationSystemTest, AdvancesAnimatorNormalizedTimeByDeltaTime) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create(animation);

  system.prepare(view);
  system.update(0.5f, view);
  const auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.normalizedTime, 0.25f);
}

TEST_F(AnimationSystemTest,
       AdvancesAnimatorNormalizedTimeByDeltaTimeAndAnimationStateSpeed) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 2.0f);
  auto entity = create(animation, true, 0.5f);

  system.prepare(view);
  system.update(0.5f, view);
  const auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.normalizedTime, 0.125f);
}

TEST_F(AnimationSystemTest,
       StaysAtTheEndIfEndOfAnimationIsReachedAndLoopModeIsNone) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create(animation);
  system.prepare(view);
  system.update(1.0f, view);
  const auto &animator = entityDatabase.get<quoll::Animator>(entity);
  EXPECT_EQ(animator.normalizedTime, 1.0f);
}

TEST_F(AnimationSystemTest,
       RestartsAnimationTimeWhenEndOfAnimationIsReachedAndLoopModeIsLinear) {
  auto animation =
      createAnimation(quoll::KeyframeSequenceAssetTarget::Position, 1.0f);
  auto entity = create(animation, true, 1.0f, quoll::AnimationLoopMode::Linear);
  system.prepare(view);
  system.update(1.0f, view);
  const auto &animator = entityDatabase.get<quoll::Animator>(entity);
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
