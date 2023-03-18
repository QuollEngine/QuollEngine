#include "liquid/core/Base.h"
#include "liquidator/actions/DeleteEntityAction.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using DeleteEntityActionTest = ActionTestBase;

TEST_P(DeleteEntityActionTest,
       ExecuteAddsDeleteComponentToEntityInSceneIfWorkspaceModeIsEdit) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::DeleteEntityAction action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Delete>(entity));
  ASSERT_EQ(res.entitiesToDelete.size(), 1);
  EXPECT_EQ(res.entitiesToDelete.at(0), entity);
}

TEST_P(DeleteEntityActionTest,
       ExecuteSetsSelectedEntityToNullIfSelectedEntityIsDeletedEntity) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;
  auto entity = activeScene().entityDatabase.create();
  state.selectedEntity = entity;

  liquid::editor::DeleteEntityAction action(entity);
  auto res = action.onExecute(state);

  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(
    DeleteEntityActionTest,
    ExecuteSetsSelectedEntityToNullIfSelectedEntityIsADescendantOfDeletedEntity) {
  auto entity = activeScene().entityDatabase.create();

  {
    auto e1 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<liquid::Parent>(e1, {entity});

    auto e2 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<liquid::Parent>(e2, {e1});
    state.selectedEntity = e2;
  }

  liquid::editor::DeleteEntityAction action(entity);
  auto res = action.onExecute(state);

  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(
    DeleteEntityActionTest,
    ExecuteDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotProvidedEntity) {
  auto entity = activeScene().entityDatabase.create();
  state.selectedEntity = activeScene().entityDatabase.create();

  liquid::editor::DeleteEntityAction action(entity);
  auto res = action.onExecute(state);

  EXPECT_NE(state.selectedEntity, liquid::Entity::Null);
}

InitActionsTestSuite(EntityActionsTest, DeleteEntityActionTest);
