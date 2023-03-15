#include "liquid/core/Base.h"
#include "liquidator/actions/EntitySkeletonActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntityToggleSkeletonDebugBonesActionTest = ActionTestBase;

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       ExecutorSetsDebugBonesForEntityIfNoDebugBonesInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Skeleton>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(state.scene.entityDatabase.has<liquid::SkeletonDebug>(entity));
  EXPECT_TRUE(res.entitiesToSave.empty());
}

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       ExecutorSetsDebugBonesForEntityIfNoDebugBonesInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::Skeleton>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(
      state.simulationScene.entityDatabase.has<liquid::SkeletonDebug>(entity));
  EXPECT_TRUE(res.entitiesToSave.empty());
}

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       ExecutorRemovesDebugBonesForEntityIfHasDebugBonesInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Skeleton>(entity, {});
  state.scene.entityDatabase.set<liquid::SkeletonDebug>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(state.scene.entityDatabase.has<liquid::SkeletonDebug>(entity));
  EXPECT_TRUE(res.entitiesToSave.empty());
}

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       ExecutorRemovesDebugBonesForEntityIfHasDebugBonesInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::Skeleton>(entity, {});
  state.simulationScene.entityDatabase.set<liquid::SkeletonDebug>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(
      state.simulationScene.entityDatabase.has<liquid::SkeletonDebug>(entity));
  EXPECT_TRUE(res.entitiesToSave.empty());
}

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       PredicateReturnsFalseIfEntityHasNoSkeletonInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       PredicateReturnsFalseIfEntityHasNoSkeletonInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       PredicateReturnsTrueIfEntityHasSkeletonInScene) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<liquid::Skeleton>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_F(EntityToggleSkeletonDebugBonesActionTest,
       PredicateReturnsTrueIfEntityHasSkeletonInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = state.simulationScene.entityDatabase.create();
  state.simulationScene.entityDatabase.set<liquid::Skeleton>(entity, {});

  liquid::editor::EntityToggleSkeletonDebugBones action(entity);
  EXPECT_TRUE(action.predicate(state));
}
