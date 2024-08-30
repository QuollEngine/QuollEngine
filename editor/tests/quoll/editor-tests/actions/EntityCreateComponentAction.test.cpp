#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/editor/actions/EntityCreateComponentAction.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntityCreateComponentActionTest = ActionTestBase;

TEST_F(EntityCreateComponentActionTest,
       PredicateReturnsFalseIfEntityHasProvidedComponent) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity);

  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityCreateComponentActionTest,
       PredicateReturnsTrueIfEntityDoesNotHaveProvidedComponent) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity);

  EXPECT_TRUE(action.predicate(state, assetCache));
}

TEST_F(EntityCreateComponentActionTest, ExecutorCreatesComponentForEntity) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity, {"Hello"});

  auto res = action.onExecute(state, assetCache);
  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Name>(entity).name, "Hello");
}

TEST_F(EntityCreateComponentActionTest, UndoDeletesComponentFromEntity) {
  state.scene.entityDatabase.create();

  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity, {"Hello"});

  action.onExecute(state, assetCache);
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Name>(entity));

  auto res = action.onUndo(state, assetCache);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Name>(entity));
}
