#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/editor/actions/EntityUpdateImmediateComponentAction.h"
#include "quoll-tests/Testing.h"
#include "ActionTestBase.h"

using EntityUpdateImmediateComponentActionTest = ActionTestBase;

TEST_F(EntityUpdateImmediateComponentActionTest,
       PredicateReturnsFalseIfEntityDoesNotHaveComponent) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityUpdateImmediateComponent<quoll::Name> action(
      entity, {"New name"});

  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityUpdateImmediateComponentActionTest,
       PredicateReturnsTrueIfEntityHasPreviousComponent) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Old name"});
  quoll::editor::EntityUpdateImmediateComponent<quoll::Name> action(entity, {});

  EXPECT_TRUE(action.predicate(state, assetCache));
}

TEST_F(EntityUpdateImmediateComponentActionTest,
       ExecutorUpdatesEntityComponentWithNewValue) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Old name"});
  quoll::editor::EntityUpdateImmediateComponent<quoll::Name> action(
      entity, {"New name"});

  auto res = action.onExecute(state, assetCache);
  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name,
            "New name");
}

TEST_F(EntityUpdateImmediateComponentActionTest,
       UndoUpdatesEntityComponentWithPreviousValue) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Old name"});
  quoll::editor::EntityUpdateImmediateComponent<quoll::Name> action(
      entity, quoll::Name{"New name"});

  action.onExecute(state, assetCache);
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name,
            "New name");

  auto res = action.onUndo(state, assetCache);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name,
            "Old name");
}
