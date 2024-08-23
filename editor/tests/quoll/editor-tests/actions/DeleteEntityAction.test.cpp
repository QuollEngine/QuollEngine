#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/scene/Parent.h"
#include "quoll/editor/actions/DeleteEntityAction.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using DeleteEntityActionTest = ActionTestBase;

TEST_F(DeleteEntityActionTest,
       ExecuteAddsDeleteComponentToEntityInSceneIfWorkspaceModeIsEdit) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(entity.has<quoll::Delete>());
  ASSERT_EQ(res.entitiesToDelete.size(), 1);
  EXPECT_EQ(res.entitiesToDelete.at(0), entity);
}

TEST_F(DeleteEntityActionTest,
       ExecuteSetsSelectedEntityToNullIfSelectedEntityIsDeletedEntity) {
  auto entity = state.scene.entityDatabase.entity();
  state.selectedEntity = entity;

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(state.selectedEntity.is_valid());
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteSetsSelectedEntityToNullIfSelectedEntityIsADescendantOfDeletedEntity) {
  auto entity = state.scene.entityDatabase.entity();

  {
    auto e1 = state.scene.entityDatabase.entity();
    e1.set<quoll::Parent>({entity});

    auto e2 = state.scene.entityDatabase.entity();
    e2.set<quoll::Parent>({e1});
    state.selectedEntity = e2;
  }

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(state.selectedEntity.is_valid());
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotProvidedEntity) {
  auto entity = state.scene.entityDatabase.entity();
  state.selectedEntity = state.scene.entityDatabase.entity();

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(state.selectedEntity.is_valid());
}

TEST_F(DeleteEntityActionTest,
       ExecuteSetsStartingCameraToDummyCameraIfStartingCameraIsDeletedEntity) {
  state.scene.dummyCamera = state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();
  state.scene.activeCamera = entity;

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.scene.activeCamera, state.scene.dummyCamera);
  EXPECT_TRUE(res.saveScene);
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteSetsStartingCameraToDummyCameraIfStartingCameraIsADescendantOfDeletedEntity) {
  state.scene.dummyCamera = state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();

  {
    auto e1 = state.scene.entityDatabase.entity();
    e1.set<quoll::Parent>({entity});

    auto e2 = state.scene.entityDatabase.entity();
    e2.set<quoll::Parent>({e1});
    state.scene.activeCamera = e2;
  }

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.scene.activeCamera, state.scene.dummyCamera);
  EXPECT_TRUE(res.saveScene);
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteDoesNotSetStartingCameraToDummyCameraIfStartingCameraIsNotAffectedByDelete) {
  state.scene.dummyCamera = state.scene.entityDatabase.entity();
  state.scene.activeCamera = state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_NE(state.scene.activeCamera, state.scene.dummyCamera);
  EXPECT_FALSE(res.saveScene);
}

TEST_F(DeleteEntityActionTest,
       ExecuteSetsActiveCameraToWorkspaceCameraIfActiveCameraIsDeletedEntity) {
  state.camera = state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();
  state.activeCamera = entity;

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.activeCamera, state.camera);
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteSetsActiveCameraToWorkspaceCameraIfActiveCameraIsADescendantOfDeletedEntity) {
  state.camera = state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();

  {
    auto e1 = state.scene.entityDatabase.entity();
    e1.set<quoll::Parent>({entity});

    auto e2 = state.scene.entityDatabase.entity();
    e2.set<quoll::Parent>({e1});
    state.activeCamera = e2;
  }

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.activeCamera, state.camera);
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteDoesNotSetActiveCameraToWorkspaceCameraIfActiveCameraIsNotAffectedByDelete) {
  state.camera = state.scene.entityDatabase.entity();
  state.activeCamera = state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_NE(state.activeCamera, state.camera);
}

TEST_F(DeleteEntityActionTest,
       ExecuteSetsEnvironmentToDummyEnvironmentIfEnvironmnetIsDeletedEntity) {
  state.scene.dummyEnvironment = state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();
  state.scene.activeEnvironment = entity;

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.scene.activeEnvironment, state.scene.dummyEnvironment);
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteSetsEnvironmentToDummyEnvironmentEnvironmentIsADescendantOfDeletedEntity) {
  state.scene.dummyEnvironment = state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();

  {
    auto e1 = state.scene.entityDatabase.entity();
    e1.set<quoll::Parent>({entity});

    auto e2 = state.scene.entityDatabase.entity();
    e2.set<quoll::Parent>({e1});
    state.scene.activeEnvironment = e2;
  }

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.scene.activeEnvironment, state.scene.dummyEnvironment);
  EXPECT_TRUE(res.saveScene);
}

TEST_F(
    DeleteEntityActionTest,
    ExecuteDoesNotSetEnvironmentToDummyEnvironmentIfEnvironmentIsNotAffectedByDelete) {
  state.scene.dummyEnvironment = state.scene.entityDatabase.entity();
  state.scene.activeEnvironment = state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::DeleteEntity action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_NE(state.scene.activeEnvironment, state.scene.dummyEnvironment);
  EXPECT_FALSE(res.saveScene);
}
