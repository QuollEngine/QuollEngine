#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/editor/actions/EntityDeleteComponentAction.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using EntityDeleteComponentActionTest = ActionTestBase;

TEST_P(EntityDeleteComponentActionTest,
       PredicateReturnsTrueIfEntityHasProvidedComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_P(EntityDeleteComponentActionTest,
       PredicateReturnsFalseIfEntityDoesNotHaveProvidedComponent) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityDeleteComponentActionTest, ExecutorDeletesComponentFromEntity) {
  activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  auto res = action.onExecute(state, assetRegistry);
  EXPECT_TRUE(res.addToHistory);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Name>(entity));
}

TEST_P(EntityDeleteComponentActionTest, UndoAddsPreviousComponentToEntity) {
  activeScene().entityDatabase.create();

  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Name>(entity, {"Hello"});
  quoll::editor::EntityDeleteComponent<quoll::Name> action(entity);

  action.onExecute(state, assetRegistry);
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Name>(entity));

  auto res = action.onUndo(state, assetRegistry);
  EXPECT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Name>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Name>(entity).name,
            "Hello");
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteComponentActionTest);
