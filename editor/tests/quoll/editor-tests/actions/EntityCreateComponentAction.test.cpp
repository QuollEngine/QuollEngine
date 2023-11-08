#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/editor/actions/EntityCreateComponentAction.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntityCreateComponentActionTest = ActionTestBase;

TEST_P(EntityCreateComponentActionTest,
       PredicateReturnsFalseIfEntityHasProvidedComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity);

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateComponentActionTest,
       PredicateReturnsTrueIfEntityDoesNotHaveProvidedComponent) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity);

  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateComponentActionTest, ExecutorCreatesComponentForEntity) {
  activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity, {"Hello"});

  auto res = action.onExecute(state, assetRegistry);
  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Name>(entity).name,
            "Hello");
}

TEST_P(EntityCreateComponentActionTest, UndoDeletesComponentFromEntity) {
  activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateComponent<quoll::Name> action(entity, {"Hello"});

  action.onExecute(state, assetRegistry);
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Name>(entity));

  auto res = action.onUndo(state, assetRegistry);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Name>(entity));
}

InitActionsTestSuite(EntityActionsTest, EntityCreateComponentActionTest);
