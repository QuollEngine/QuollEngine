#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntitySkeletonActions.h"
#include "quoll-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityToggleSkeletonDebugBonesActionTest = ActionTestBase;

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       ExecutorSetsDebugBonesForEntityIfNoDebugBones) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Skeleton>(entity, {});

  quoll::editor::EntityToggleSkeletonDebugBones action(entity);
  auto res = action.onExecute(state, assetCache);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::SkeletonDebug>(entity));
  EXPECT_TRUE(res.entitiesToSave.empty());
}

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       ExecutorRemovesDebugBonesForEntityIfHasDebugBones) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Skeleton>(entity, {});
  state.scene.entityDatabase.set<quoll::SkeletonDebug>(entity, {});

  quoll::editor::EntityToggleSkeletonDebugBones action(entity);
  auto res = action.onExecute(state, assetCache);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::SkeletonDebug>(entity));
  EXPECT_TRUE(res.entitiesToSave.empty());
}

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       PredicateReturnsFalseIfEntityHasNoSkeleton) {
  auto entity = state.scene.entityDatabase.create();

  quoll::editor::EntityToggleSkeletonDebugBones action(entity);
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       PredicateReturnsTrueIfEntityHasSkeleton) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Skeleton>(entity, {});

  quoll::editor::EntityToggleSkeletonDebugBones action(entity);
  EXPECT_TRUE(action.predicate(state, assetCache));
}

using EntityDeleteSkeletonActionTest = ActionTestBase;

InitDefaultDeleteComponentTests(EntityDeleteSkeletonActionTest,
                                EntityDeleteSkeleton, Skeleton);

TEST_F(EntityDeleteSkeletonActionTest,
       ExecutorDeletesSkeletonDebugComponentFromEntity) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Skeleton>(entity, {});
  state.scene.entityDatabase.set<quoll::SkeletonDebug>(entity, {});

  quoll::editor::EntityDeleteSkeleton action(entity);
  auto res = action.onExecute(state, assetCache);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Skeleton>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::SkeletonDebug>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(
    EntityDeleteSkeletonActionTest,
    UndoDoesNotCreateSkeletonDebugComponentForEntityIfItDidNotExistDuringExecution) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Skeleton>(entity, {});

  quoll::editor::EntityDeleteSkeleton action(entity);
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::SkeletonDebug>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityDeleteSkeletonActionTest,
       UndoCretesSkeletonDebugComponentForEntityIfItExistedDuringExecution) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Skeleton>(entity, {});
  state.scene.entityDatabase.set<quoll::SkeletonDebug>(entity, {});

  quoll::editor::EntityDeleteSkeleton action(entity);
  action.onExecute(state, assetCache);

  auto res = action.onUndo(state, assetCache);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::SkeletonDebug>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
