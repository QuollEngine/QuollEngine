#include "quoll/core/Base.h"
#include "quoll/scene/AutoAspectRatio.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/editor/actions/EntityCameraActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreatePerspectiveLensActionTest = ActionTestBase;

TEST_F(EntityCreatePerspectiveLensActionTest,
       ExecutorCreatesLensCameraAndAutoAspectRatioForEntity) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreatePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::PerspectiveLens>(entity));
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Camera>(entity));
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::AutoAspectRatio>(entity));

  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityCreatePerspectiveLensActionTest,
       UndoDeletesLensCameraAndAutoAspectRatioFromEntity) {
  auto entity = state.scene.entityDatabase.create();

  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<quoll::Camera>(entity, {});
  state.scene.entityDatabase.set<quoll::AutoAspectRatio>(entity, {});

  quoll::editor::EntityCreatePerspectiveLens action(entity);
  auto res = action.onUndo(state, assetRegistry);
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::PerspectiveLens>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityCreatePerspectiveLensActionTest,
       PredicateReturnsFalseIfComponentAlreadyExists) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  EXPECT_FALSE(quoll::editor::EntityCreatePerspectiveLens(entity).predicate(
      state, assetRegistry));
}

TEST_F(EntityCreatePerspectiveLensActionTest,
       PredicateReturnsTrueIfComponentDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();
  EXPECT_TRUE(quoll::editor::EntityCreatePerspectiveLens(entity).predicate(
      state, assetRegistry));
}

using EntityDeletePerspectiveLensActionTest = ActionTestBase;

InitDefaultDeleteComponentTests(EntityDeletePerspectiveLensActionTest,
                                EntityDeletePerspectiveLens, PerspectiveLens);

TEST_F(EntityDeletePerspectiveLensActionTest,
       ExecutorDeletesCameraAndAutoAspectRatioComponents) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<quoll::Camera>(entity, {});
  state.scene.entityDatabase.set<quoll::AutoAspectRatio>(entity, {});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Camera>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::AutoAspectRatio>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_F(EntityDeletePerspectiveLensActionTest, UndoCreatesCameraComponent) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  // Camera will be created even if it does
  // not exist on the entity
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Camera>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       UndoCreatesAutoAspectRatioIfItExistedDuringExecution) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<quoll::AutoAspectRatio>(entity, {});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::AutoAspectRatio>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       UndoDoesNotCreateAutoAspectRatioIfItDidNotExistDuringExecution) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::AutoAspectRatio>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       ExecutorSetsStartingCameraToDummyCameraIfEntityIsStartingCamera) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.dummyCamera = state.scene.entityDatabase.create();
  state.scene.activeCamera = entity;

  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<quoll::Camera>(entity, {});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::PerspectiveLens>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.saveScene);

  EXPECT_EQ(state.scene.activeCamera, state.scene.dummyCamera);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       UndoSetsStartingCameraToEntityIfEntityIsStartingCamera) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.dummyCamera = state.scene.entityDatabase.create();
  state.scene.activeCamera = entity;

  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<quoll::Camera>(entity, {});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.saveScene);

  EXPECT_EQ(state.scene.activeCamera, entity);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       ExecutorSetsWorkspaceActiveCameraToDummyCameraIfEntityIsActiveCamera) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.dummyCamera = state.scene.entityDatabase.create();
  state.activeCamera = entity;

  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<quoll::Camera>(entity, {});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::PerspectiveLens>(entity));

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);

  EXPECT_EQ(state.activeCamera, state.scene.dummyCamera);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       UndoSetsWorkspaceActiveCameraToEntityIfEntityIsActiveCamera) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.dummyCamera = state.scene.entityDatabase.create();
  state.activeCamera = entity;

  state.scene.entityDatabase.set<quoll::PerspectiveLens>(entity, {});
  state.scene.entityDatabase.set<quoll::Camera>(entity, {});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);

  EXPECT_EQ(state.activeCamera, entity);
}
