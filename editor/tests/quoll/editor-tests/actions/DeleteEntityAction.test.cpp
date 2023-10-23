#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/scene/Parent.h"

#include "quoll/editor/actions/DeleteEntityAction.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using DeleteEntityActionTest = ActionTestBase;

TEST_P(DeleteEntityActionTest,
       ExecuteAddsDeleteComponentToEntityInSceneIfWorkspaceModeIsEdit) {
  auto entity = activeScene().entityDatabase.create();

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Delete>(entity));
  ASSERT_EQ(res.entitiesToDelete.size(), 1);
  EXPECT_EQ(res.entitiesToDelete.at(0), entity);
}

TEST_P(DeleteEntityActionTest,
       ExecuteSetsSelectedEntityToNullIfSelectedEntityIsDeletedEntity) {
  auto entity = activeScene().entityDatabase.create();
  state.selectedEntity = entity;

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(
    DeleteEntityActionTest,
    ExecuteSetsSelectedEntityToNullIfSelectedEntityIsADescendantOfDeletedEntity) {
  auto entity = activeScene().entityDatabase.create();

  {
    auto e1 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e1, {entity});

    auto e2 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e2, {e1});
    state.selectedEntity = e2;
  }

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(
    DeleteEntityActionTest,
    ExecuteDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotProvidedEntity) {
  auto entity = activeScene().entityDatabase.create();
  state.selectedEntity = activeScene().entityDatabase.create();

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_NE(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(DeleteEntityActionTest,
       ExecuteSetsStartingCameraToDummyCameraIfStartingCameraIsDeletedEntity) {
  activeScene().dummyCamera = activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();
  activeScene().activeCamera = entity;

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().activeCamera, activeScene().dummyCamera);
  EXPECT_TRUE(res.saveScene);
}

TEST_P(
    DeleteEntityActionTest,
    ExecuteSetsStartingCameraToDummyCameraIfStartingCameraIsADescendantOfDeletedEntity) {
  activeScene().dummyCamera = activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();

  {
    auto e1 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e1, {entity});

    auto e2 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e2, {e1});
    activeScene().activeCamera = e2;
  }

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().activeCamera, activeScene().dummyCamera);
  EXPECT_TRUE(res.saveScene);
}

TEST_P(
    DeleteEntityActionTest,
    ExecuteDoesNotSetStartingCameraToDummyCameraIfStartingCameraIsNotAffectedByDelete) {
  activeScene().dummyCamera = activeScene().entityDatabase.create();
  activeScene().activeCamera = activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_NE(activeScene().activeCamera, activeScene().dummyCamera);
  EXPECT_FALSE(res.saveScene);
}

TEST_P(DeleteEntityActionTest,
       ExecuteSetsActiveCameraToWorkspaceCameraIfActiveCameraIsDeletedEntity) {
  state.camera = activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();
  state.activeCamera = entity;

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.activeCamera, state.camera);
}

TEST_P(
    DeleteEntityActionTest,
    ExecuteSetsActiveCameraToWorkspaceCameraIfActiveCameraIsADescendantOfDeletedEntity) {
  state.camera = activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();

  {
    auto e1 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e1, {entity});

    auto e2 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e2, {e1});
    state.activeCamera = e2;
  }

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.activeCamera, state.camera);
}

TEST_P(
    DeleteEntityActionTest,
    ExecuteDoesNotSetActiveCameraToWorkspaceCameraIfActiveCameraIsNotAffectedByDelete) {
  state.camera = activeScene().entityDatabase.create();
  state.activeCamera = activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_NE(state.activeCamera, state.camera);
}

TEST_P(DeleteEntityActionTest,
       ExecuteSetsEnvironmentToDummyEnvironmentIfEnvironmnetIsDeletedEntity) {
  activeScene().dummyEnvironment = activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();
  activeScene().activeEnvironment = entity;

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().activeEnvironment, activeScene().dummyEnvironment);
}

TEST_P(
    DeleteEntityActionTest,
    ExecuteSetsEnvironmentToDummyEnvironmentEnvironmentIsADescendantOfDeletedEntity) {
  activeScene().dummyEnvironment = activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();

  {
    auto e1 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e1, {entity});

    auto e2 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e2, {e1});
    activeScene().activeEnvironment = e2;
  }

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().activeEnvironment, activeScene().dummyEnvironment);
  EXPECT_TRUE(res.saveScene);
}

TEST_P(
    DeleteEntityActionTest,
    ExecuteDoesNotSetEnvironmentToDummyEnvironmentIfEnvironmentIsNotAffectedByDelete) {
  activeScene().dummyEnvironment = activeScene().entityDatabase.create();
  activeScene().activeEnvironment = activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_NE(activeScene().activeEnvironment, activeScene().dummyEnvironment);
  EXPECT_FALSE(res.saveScene);
}

InitActionsTestSuite(EntityActionsTest, DeleteEntityActionTest);
