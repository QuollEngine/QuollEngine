#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/editor/actions/EntityCreateComponentAction.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntityCreateComponentActionTest = ActionTestBase;

TEST_F(EntityCreateComponentActionTest,
       PredicateReturnsFalseIfEntityHasProvidedComponent) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::Name>({"Hello"});
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity);

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityCreateComponentActionTest,
       PredicateReturnsTrueIfEntityDoesNotHaveProvidedComponent) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity);

  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_F(EntityCreateComponentActionTest, ExecutorCreatesComponentForEntity) {
  state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity, {"Hello"});

  auto res = action.onExecute(state, assetRegistry);
  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(entity.has<quoll::Name>());
  EXPECT_EQ(entity.get_ref<quoll::Name>()->name, "Hello");
}

TEST_F(EntityCreateComponentActionTest, UndoDeletesComponentFromEntity) {
  state.scene.entityDatabase.entity();

  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity, {"Hello"});

  action.onExecute(state, assetRegistry);
  EXPECT_TRUE(entity.has<quoll::Name>());

  auto res = action.onUndo(state, assetRegistry);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(entity.has<quoll::Name>());
}
