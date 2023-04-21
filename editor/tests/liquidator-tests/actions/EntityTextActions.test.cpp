#include "liquid/core/Base.h"
#include "liquidator/actions/EntityTextActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetTextActionTest = ActionTestBase;

TEST_P(EntitySetTextActionTest, ExecutorSetsTextForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetText action(entity, liquid::Text{"Hello world"});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Text>(entity).text,
            "Hello world");
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetTextActionTest);

using EntityDeleteTextActionTest = ActionTestBase;

TEST_P(EntityDeleteTextActionTest, ExecutorDeletesTextComponentFromEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Text>(entity, {});

  liquid::editor::EntityDeleteText action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Text>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteTextActionTest,
       PredicateReturnsTrueIfEntityHasTextComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Text>(entity, {});

  liquid::editor::EntityDeleteText action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntityDeleteTextActionTest,
       PredicateReturnsTrueIfEntityHasNoTextComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityDeleteText action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteTextActionTest);
