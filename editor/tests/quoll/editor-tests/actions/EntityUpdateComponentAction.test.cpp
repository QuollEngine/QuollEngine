#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/editor/actions/EntityUpdateComponentAction.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntityUpdateComponentActionTest = ActionTestBase;

TEST_F(EntityUpdateComponentActionTest,
       PredicateReturnsFalseIfNewComponentIsNotProvided) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Old name"});

  quoll::editor::EntityUpdateComponent<quoll::Name> action(entity,
                                                           {"Old name"});

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityUpdateComponentActionTest,
       PredicateReturnsFalseIfEntityDoesNotHaveComponent) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityUpdateComponent<quoll::Name> action(
      entity, {"Old name"}, quoll::Name{"New name"});

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityUpdateComponentActionTest,
       PredicateReturnsTrueIfNewComponentIsProvided) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Old name"});
  quoll::editor::EntityUpdateComponent<quoll::Name> action(
      entity, {"Old name"}, quoll::Name{"New name"});

  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_F(EntityUpdateComponentActionTest,
       ExecutorUpdatesEntityComponentWithNewValue) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Old name"});
  quoll::editor::EntityUpdateComponent<quoll::Name> action(
      entity, {"Old name"}, quoll::Name{"New name"});

  auto res = action.onExecute(state, assetRegistry);
  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name,
            "New name");
}

TEST_F(EntityUpdateComponentActionTest,
       UndoUpdatesEntityComponentWithPreviousValue) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Old name"});
  quoll::editor::EntityUpdateComponent<quoll::Name> action(
      entity, {"Old name"}, quoll::Name{"New name"});

  action.onExecute(state, assetRegistry);
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name,
            "New name");

  auto res = action.onUndo(state, assetRegistry);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name,
            "Old name");
}
