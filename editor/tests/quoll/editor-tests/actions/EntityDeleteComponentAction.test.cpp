#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/editor/actions/EntityDeleteComponentAction.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntityDeleteComponentActionTest = ActionTestBase;

TEST_F(EntityDeleteComponentActionTest,
       PredicateReturnsTrueIfEntityHasProvidedComponent) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  EXPECT_TRUE(action.predicate(state, assetCache));
}

TEST_F(EntityDeleteComponentActionTest,
       PredicateReturnsFalseIfEntityDoesNotHaveProvidedComponent) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityDeleteComponentActionTest, ExecutorDeletesComponentFromEntity) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  auto res = action.onExecute(state, assetCache);
  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Name>(entity));
}

TEST_F(EntityDeleteComponentActionTest, UndoAddsPreviousComponentToEntity) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  action.onExecute(state, assetCache);
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Name>(entity));

  auto res = action.onUndo(state, assetCache);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name, "Hello");
}
