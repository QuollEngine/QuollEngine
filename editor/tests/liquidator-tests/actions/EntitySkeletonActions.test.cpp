#include "liquid/core/Base.h"
#include "liquidator/actions/EntitySkeletonActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntityToggleSkeletonDebugBonesActionTest = ActionTestBase;

TEST_P(EntityToggleSkeletonDebugBonesActionTest,
       ExecutorSetsDebugBonesForEntityIfNoDebugBones) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Skeleton>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::SkeletonDebug>(entity));
  EXPECT_TRUE(res.entitiesToSave.empty());
}

TEST_P(EntityToggleSkeletonDebugBonesActionTest,
       ExecutorRemovesDebugBonesForEntityIfHasDebugBones) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Skeleton>(entity, {});
  activeScene().entityDatabase.set<liquid::SkeletonDebug>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::SkeletonDebug>(entity));
  EXPECT_TRUE(res.entitiesToSave.empty());
}

TEST_P(EntityToggleSkeletonDebugBonesActionTest,
       PredicateReturnsFalseIfEntityHasNoSkeleton) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntityToggleSkeletonDebugBonesActionTest,
       PredicateReturnsTrueIfEntityHasSkeleton) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Skeleton>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest,
                     EntityToggleSkeletonDebugBonesActionTest);
