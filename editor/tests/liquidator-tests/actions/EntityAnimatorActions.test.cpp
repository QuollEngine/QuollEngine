#include "liquid/core/Base.h"
#include "liquidator/actions/EntityAnimatorActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntityDeleteAnimatorActionTest = ActionTestBase;

TEST_P(EntityDeleteAnimatorActionTest,
       ExecutorDeletesAnimatorComponentFromEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Animator>(entity, {});

  liquid::editor::EntityDeleteAnimator action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Animator>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteAnimatorActionTest,
       PredicateReturnsTrueIfEntityHasAnimatorComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Animator>(entity, {});

  liquid::editor::EntityDeleteAnimator action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntityDeleteAnimatorActionTest,
       PredicateReturnsTrueIfEntityHasNoAnimatorComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityDeleteAnimator action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteAnimatorActionTest);
