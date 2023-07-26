#include "liquid/core/Base.h"
#include "liquidator/actions/EntityCameraActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreatePerspectiveLensActionTest = ActionTestBase;

TEST_P(EntityCreatePerspectiveLensActionTest,
       ExecutorCreatesLensCameraAndAutoAspectRatioForEntity) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntityCreatePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);
  EXPECT_TRUE(
      activeScene().entityDatabase.has<liquid::PerspectiveLens>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Camera>(entity));
  EXPECT_TRUE(
      activeScene().entityDatabase.has<liquid::AutoAspectRatio>(entity));

  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityCreatePerspectiveLensActionTest,
       UndoDeletesLensCameraAndAutoAspectRatioFromEntity) {
  auto entity = activeScene().entityDatabase.create();

  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  activeScene().entityDatabase.set<liquid::Camera>(entity, {});
  activeScene().entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntityCreatePerspectiveLens action(entity);
  auto res = action.onUndo(state, assetRegistry);
  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::PerspectiveLens>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityCreatePerspectiveLensActionTest,
       PredicateReturnsFalseIfComponentAlreadyExists) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  EXPECT_FALSE(liquid::editor::EntityCreatePerspectiveLens(entity).predicate(
      state, assetRegistry));
}

TEST_P(EntityCreatePerspectiveLensActionTest,
       PredicateReturnsTrueIfComponentDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();
  EXPECT_TRUE(liquid::editor::EntityCreatePerspectiveLens(entity).predicate(
      state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntityCreatePerspectiveLensActionTest);

using EntitySetPerspectiveLensActionTest = ActionTestBase;

InitDefaultUpdateComponentTests(EntitySetPerspectiveLensActionTest,
                                EntitySetPerspectiveLens, PerspectiveLens,
                                focalLength, 45.0f);

InitActionsTestSuite(EntityActionsTest, EntitySetPerspectiveLensActionTest);

using EntitySetCameraAutoAspectRatioTest = ActionTestBase;
InitDefaultCreateComponentTests(EntitySetCameraAutoAspectRatioTest,
                                EntitySetCameraAutoAspectRatio,
                                AutoAspectRatio);
InitActionsTestSuite(EntityActionsTest, EntitySetCameraAutoAspectRatioTest);

using SetEntityCameraCustomAspectRatioTest = ActionTestBase;
InitDefaultDeleteComponentTests(SetEntityCameraCustomAspectRatioTest,
                                EntitySetCameraCustomAspectRatio,
                                AutoAspectRatio);
InitActionsTestSuite(EntityActionsTest, SetEntityCameraCustomAspectRatioTest);

using EntityDeletePerspectiveLensActionTest = ActionTestBase;

InitDefaultDeleteComponentTests(EntityDeletePerspectiveLensActionTest,
                                EntityDeletePerspectiveLens, PerspectiveLens);

TEST_P(EntityDeletePerspectiveLensActionTest,
       ExecutorDeletesCameraAndAutoAspectRatioComponents) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  activeScene().entityDatabase.set<liquid::Camera>(entity, {});
  activeScene().entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntityDeletePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Camera>(entity));
  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::AutoAspectRatio>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_P(EntityDeletePerspectiveLensActionTest, UndoCreatesCameraComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  liquid::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  // Camera will be created even if it does
  // not exist on the entity
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Camera>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_P(EntityDeletePerspectiveLensActionTest,
       UndoCreatesAutoAspectRatioIfItExistedDuringExecution) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  activeScene().entityDatabase.set<liquid::AutoAspectRatio>(entity, {});

  liquid::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(
      activeScene().entityDatabase.has<liquid::AutoAspectRatio>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_P(EntityDeletePerspectiveLensActionTest,
       UndoDoesNotCreateAutoAspectRatioIfItDidNotExistDuringExecution) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  liquid::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::AutoAspectRatio>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_P(EntityDeletePerspectiveLensActionTest,
       ExecutorSetsStartingCameraToDummyCameraIfEntityIsStartingCamera) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().dummyCamera = activeScene().entityDatabase.create();
  activeScene().activeCamera = entity;

  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  activeScene().entityDatabase.set<liquid::Camera>(entity, {});

  liquid::editor::EntityDeletePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::PerspectiveLens>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.saveScene);

  EXPECT_EQ(activeScene().activeCamera, activeScene().dummyCamera);
}

TEST_P(EntityDeletePerspectiveLensActionTest,
       UndoSetsStartingCameraToEntityIfEntityIsStartingCamera) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().dummyCamera = activeScene().entityDatabase.create();
  activeScene().activeCamera = entity;

  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  activeScene().entityDatabase.set<liquid::Camera>(entity, {});

  liquid::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.saveScene);

  EXPECT_EQ(activeScene().activeCamera, entity);
}

TEST_P(EntityDeletePerspectiveLensActionTest,
       ExecutorSetsWorkspaceActiveCameraToDummyCameraIfEntityIsActiveCamera) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().dummyCamera = activeScene().entityDatabase.create();
  state.activeCamera = entity;

  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  activeScene().entityDatabase.set<liquid::Camera>(entity, {});

  liquid::editor::EntityDeletePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::PerspectiveLens>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);

  EXPECT_EQ(state.activeCamera, activeScene().dummyCamera);
}

TEST_P(EntityDeletePerspectiveLensActionTest,
       UndoSetsWorkspaceActiveCameraToEntityIfEntityIsActiveCamera) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().dummyCamera = activeScene().entityDatabase.create();
  state.activeCamera = entity;

  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  activeScene().entityDatabase.set<liquid::Camera>(entity, {});

  liquid::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);

  EXPECT_EQ(state.activeCamera, entity);
}

InitActionsTestSuite(EntityActionsTest, EntityDeletePerspectiveLensActionTest);
