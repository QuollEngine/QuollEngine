#include "liquid/core/Base.h"
#include "liquidator/actions/SceneActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using SceneSetStartingCameraActionTest = ActionTestBase;

TEST_P(SceneSetStartingCameraActionTest, ExecutorSetsActiveCamera) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::SceneSetStartingCamera action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().activeCamera, entity);
  EXPECT_TRUE(res.saveScene);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SceneSetStartingCameraActionTest, UndoSetsPreviousActiveCamera) {
  auto entity = activeScene().entityDatabase.create();

  auto cameraEntity = activeScene().entityDatabase.create();
  activeScene().activeCamera = cameraEntity;

  liquid::editor::SceneSetStartingCamera action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(activeScene().activeCamera, cameraEntity);
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneSetStartingCameraActionTest,
       PredicateReturnsFalseIfProvidedEntityEqualsActiveCamera) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  activeScene().activeCamera = entity;

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(SceneSetStartingCameraActionTest,
       PredicateReturnsFalseIfProvidedEntityDoesNotHavePerspectiveLens) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(SceneSetStartingCameraActionTest,
       PredicateReturnsTrueIfProvidedCameraEntityIsDifferentThanActiveCamera) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(SceneActionsTest, SceneSetStartingCameraActionTest);

using SceneSetStartingEnvironmentActionTest = ActionTestBase;

TEST_P(SceneSetStartingEnvironmentActionTest, ExecutorSetsActiveEnvironment) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::SceneSetStartingEnvironment action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().activeEnvironment, entity);
  EXPECT_TRUE(res.saveScene);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SceneSetStartingEnvironmentActionTest,
       UndoSetsPreviousActiveEnvironment) {
  auto entity = activeScene().entityDatabase.create();

  auto environment = activeScene().entityDatabase.create();
  activeScene().activeEnvironment = environment;

  liquid::editor::SceneSetStartingEnvironment action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(activeScene().activeEnvironment, environment);
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneSetStartingEnvironmentActionTest,
       PredicateReturnsFalseIfProvidedEntityEqualsActiveEnvironment) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().activeEnvironment = entity;

  liquid::editor::SceneSetStartingEnvironment action(entity);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(
    SceneSetStartingEnvironmentActionTest,
    PredicateReturnsTrueIfProvidedEnvironmentEntityIsDifferentThanActiveEnvironment) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::SceneSetStartingEnvironment action(entity);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(SceneActionsTest, SceneSetStartingEnvironmentActionTest);

using SceneRemoveStartingEnvironmentActionTest = ActionTestBase;

TEST_P(SceneRemoveStartingEnvironmentActionTest,
       ExecutorSetsActiveEnvironmentToDummyEnvironment) {
  activeScene().activeEnvironment = activeScene().entityDatabase.create();

  liquid::editor::SceneRemoveStartingEnvironment action;
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().activeEnvironment, activeScene().dummyEnvironment);
  EXPECT_TRUE(res.saveScene);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SceneRemoveStartingEnvironmentActionTest,
       UndoSetsPreviousEnvironmentAsActive) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().activeEnvironment = entity;

  liquid::editor::SceneRemoveStartingEnvironment action;
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(activeScene().activeEnvironment, entity);
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneRemoveStartingEnvironmentActionTest,
       PredicateReturnsFalseIfEnvironmentEntityIsDummy) {
  liquid::editor::SceneRemoveStartingEnvironment action;
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(SceneRemoveStartingEnvironmentActionTest,
       PredicateReturnsTrueIfActiveEnvironmentIsDifferentThanDummyEnvironment) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().activeEnvironment = entity;

  liquid::editor::SceneRemoveStartingEnvironment action;
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(SceneActionsTest,
                     SceneRemoveStartingEnvironmentActionTest);
