#include "quoll/core/Base.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/editor/actions/SceneActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using SceneSetStartingCameraActionTest = ActionTestBase;

TEST_F(SceneSetStartingCameraActionTest, ExecutorSetsActiveCamera) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::SceneSetStartingCamera action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.scene.activeCamera, entity);
  EXPECT_TRUE(res.saveScene);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(SceneSetStartingCameraActionTest, UndoSetsPreviousActiveCamera) {
  auto entity = state.scene.entityDatabase.entity();

  auto cameraEntity = state.scene.entityDatabase.entity();
  state.scene.activeCamera = cameraEntity;

  quoll::editor::SceneSetStartingCamera action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(state.scene.activeCamera, cameraEntity);
  EXPECT_TRUE(res.saveScene);
}

TEST_F(SceneSetStartingCameraActionTest,
       PredicateReturnsFalseIfProvidedEntityEqualsActiveCamera) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::PerspectiveLens>({});
  state.scene.activeCamera = entity;

  quoll::editor::SceneSetStartingCamera action(entity);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(SceneSetStartingCameraActionTest,
       PredicateReturnsFalseIfProvidedEntityDoesNotHavePerspectiveLens) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::SceneSetStartingCamera action(entity);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(SceneSetStartingCameraActionTest,
       PredicateReturnsTrueIfProvidedCameraEntityIsDifferentThanActiveCamera) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::PerspectiveLens>({});

  quoll::editor::SceneSetStartingCamera action(entity);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

using SceneSetStartingEnvironmentActionTest = ActionTestBase;

TEST_F(SceneSetStartingEnvironmentActionTest, ExecutorSetsActiveEnvironment) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::SceneSetStartingEnvironment action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.scene.activeEnvironment, entity);
  EXPECT_TRUE(res.saveScene);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(SceneSetStartingEnvironmentActionTest,
       UndoSetsPreviousActiveEnvironment) {
  auto entity = state.scene.entityDatabase.entity();

  auto environment = state.scene.entityDatabase.entity();
  state.scene.activeEnvironment = environment;

  quoll::editor::SceneSetStartingEnvironment action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(state.scene.activeEnvironment, environment);
  EXPECT_TRUE(res.saveScene);
}

TEST_F(SceneSetStartingEnvironmentActionTest,
       PredicateReturnsFalseIfProvidedEntityEqualsActiveEnvironment) {
  auto entity = state.scene.entityDatabase.entity();
  state.scene.activeEnvironment = entity;

  quoll::editor::SceneSetStartingEnvironment action(entity);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(
    SceneSetStartingEnvironmentActionTest,
    PredicateReturnsTrueIfProvidedEnvironmentEntityIsDifferentThanActiveEnvironment) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::SceneSetStartingEnvironment action(entity);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

using SceneRemoveStartingEnvironmentActionTest = ActionTestBase;

TEST_F(SceneRemoveStartingEnvironmentActionTest,
       ExecutorSetsActiveEnvironmentToDummyEnvironment) {
  state.scene.activeEnvironment = state.scene.entityDatabase.entity();

  quoll::editor::SceneRemoveStartingEnvironment action;
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.scene.activeEnvironment, state.scene.dummyEnvironment);
  EXPECT_TRUE(res.saveScene);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(SceneRemoveStartingEnvironmentActionTest,
       UndoSetsPreviousEnvironmentAsActive) {
  auto entity = state.scene.entityDatabase.entity();
  state.scene.activeEnvironment = entity;

  quoll::editor::SceneRemoveStartingEnvironment action;
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(state.scene.activeEnvironment, entity);
  EXPECT_TRUE(res.saveScene);
}

TEST_F(SceneRemoveStartingEnvironmentActionTest,
       PredicateReturnsFalseIfEnvironmentEntityIsDummy) {
  quoll::editor::SceneRemoveStartingEnvironment action;
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(SceneRemoveStartingEnvironmentActionTest,
       PredicateReturnsTrueIfActiveEnvironmentIsDifferentThanDummyEnvironment) {
  auto entity = state.scene.entityDatabase.entity();
  state.scene.activeEnvironment = entity;

  quoll::editor::SceneRemoveStartingEnvironment action;
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}
