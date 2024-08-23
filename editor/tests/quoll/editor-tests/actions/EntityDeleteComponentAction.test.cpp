#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/editor/actions/EntityDeleteComponentAction.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntityDeleteComponentActionTest = ActionTestBase;

TEST_F(EntityDeleteComponentActionTest,
       PredicateReturnsTrueIfEntityHasProvidedComponent) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::Name>({"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_F(EntityDeleteComponentActionTest,
       PredicateReturnsFalseIfEntityDoesNotHaveProvidedComponent) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityDeleteComponentActionTest, ExecutorDeletesComponentFromEntity) {
  state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::Name>({"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  auto res = action.onExecute(state, assetRegistry);
  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(entity.has<quoll::Name>());
}

TEST_F(EntityDeleteComponentActionTest, UndoAddsPreviousComponentToEntity) {
  state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::Name>({"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  action.onExecute(state, assetRegistry);
  EXPECT_FALSE(entity.has<quoll::Name>());

  auto res = action.onUndo(state, assetRegistry);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(entity.has<quoll::Name>());
  EXPECT_EQ(entity.get_ref<quoll::Name>()->name, "Hello");
}
