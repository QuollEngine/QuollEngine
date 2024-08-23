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
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityCreatePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);
  EXPECT_TRUE(entity.has<quoll::PerspectiveLens>());
  EXPECT_TRUE(entity.has<quoll::Camera>());
  EXPECT_TRUE(entity.has<quoll::AutoAspectRatio>());

  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityCreatePerspectiveLensActionTest,
       UndoDeletesLensCameraAndAutoAspectRatioFromEntity) {
  auto entity = state.scene.entityDatabase.entity();

  entity.set<quoll::PerspectiveLens>({});
  entity.set<quoll::Camera>({});
  entity.add<quoll::AutoAspectRatio>();

  quoll::editor::EntityCreatePerspectiveLens action(entity);
  auto res = action.onUndo(state, assetRegistry);
  EXPECT_FALSE(entity.has<quoll::PerspectiveLens>());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityCreatePerspectiveLensActionTest,
       PredicateReturnsFalseIfComponentAlreadyExists) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::PerspectiveLens>({});
  EXPECT_FALSE(quoll::editor::EntityCreatePerspectiveLens(entity).predicate(
      state, assetRegistry));
}

TEST_F(EntityCreatePerspectiveLensActionTest,
       PredicateReturnsTrueIfComponentDoesNotExist) {
  auto entity = state.scene.entityDatabase.entity();
  EXPECT_TRUE(quoll::editor::EntityCreatePerspectiveLens(entity).predicate(
      state, assetRegistry));
}

using EntityDeletePerspectiveLensActionTest = ActionTestBase;

InitDefaultDeleteComponentTests(EntityDeletePerspectiveLensActionTest,
                                EntityDeletePerspectiveLens, PerspectiveLens);

TEST_F(EntityDeletePerspectiveLensActionTest,
       ExecutorDeletesCameraAndAutoAspectRatioComponents) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::PerspectiveLens>({});
  entity.set<quoll::Camera>({});
  entity.add<quoll::AutoAspectRatio>();

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::Camera>());
  EXPECT_FALSE(entity.has<quoll::AutoAspectRatio>());

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_F(EntityDeletePerspectiveLensActionTest, UndoCreatesCameraComponent) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::PerspectiveLens>({});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  // Camera will be created even if it does
  // not exist on the entity
  EXPECT_TRUE(entity.has<quoll::Camera>());

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       UndoCreatesAutoAspectRatioIfItExistedDuringExecution) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::PerspectiveLens>({});
  entity.add<quoll::AutoAspectRatio>();

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(entity.has<quoll::AutoAspectRatio>());

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       UndoDoesNotCreateAutoAspectRatioIfItDidNotExistDuringExecution) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::PerspectiveLens>({});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::AutoAspectRatio>());

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       ExecutorSetsStartingCameraToDummyCameraIfEntityIsStartingCamera) {
  auto entity = state.scene.entityDatabase.entity();
  state.scene.dummyCamera = state.scene.entityDatabase.entity();
  state.scene.activeCamera = entity;

  entity.set<quoll::PerspectiveLens>({});
  entity.set<quoll::Camera>({});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::PerspectiveLens>());

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.saveScene);

  EXPECT_EQ(state.scene.activeCamera, state.scene.dummyCamera);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       UndoSetsStartingCameraToEntityIfEntityIsStartingCamera) {
  auto entity = state.scene.entityDatabase.entity();
  state.scene.dummyCamera = state.scene.entityDatabase.entity();
  state.scene.activeCamera = entity;

  entity.set<quoll::PerspectiveLens>({});
  entity.set<quoll::Camera>({});

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
  auto entity = state.scene.entityDatabase.entity();
  state.scene.dummyCamera = state.scene.entityDatabase.entity();
  state.activeCamera = entity;

  entity.set<quoll::PerspectiveLens>({});
  entity.set<quoll::Camera>({});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::PerspectiveLens>());

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);

  EXPECT_EQ(state.activeCamera, state.scene.dummyCamera);
}

TEST_F(EntityDeletePerspectiveLensActionTest,
       UndoSetsWorkspaceActiveCameraToEntityIfEntityIsActiveCamera) {
  auto entity = state.scene.entityDatabase.entity();
  state.scene.dummyCamera = state.scene.entityDatabase.entity();
  state.activeCamera = entity;

  entity.set<quoll::PerspectiveLens>({});
  entity.set<quoll::Camera>({});

  quoll::editor::EntityDeletePerspectiveLens action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(res.saveScene);

  EXPECT_EQ(state.activeCamera, entity);
}
