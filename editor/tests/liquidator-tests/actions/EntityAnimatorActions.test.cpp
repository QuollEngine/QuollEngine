#include "liquid/core/Base.h"
#include "liquidator/actions/EntityAnimatorActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateAnimatorActionTest = ActionTestBase;

TEST_P(EntityCreateAnimatorActionTest, ExecutorCreatesAnimatorSourceComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityCreateAnimator action(entity,
                                              liquid::AnimatorAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Animator>(entity).asset,
            liquid::AnimatorAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityCreateAnimatorActionTest, UndoDeletesAnimatorSourceComponet) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Animator>(
      entity, {liquid::AnimatorAssetHandle{25}});

  liquid::editor::EntityCreateAnimator action(entity,
                                              liquid::AnimatorAssetHandle{15});
  auto res = action.onUndo(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Animator>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityCreateAnimatorActionTest,
       PredicateReturnsFalseIfAnimatorAssetIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntityCreateAnimator action(entity,
                                              liquid::AnimatorAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntityCreateAnimatorActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasAnimator) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntityCreateAnimator action(entity,
                                              liquid::AnimatorAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntityCreateAnimatorActionTest,
       PredicateReturnsTrueIfAnimatorDoesNotExistAndAssetIsValid) {
  auto entity = activeScene().entityDatabase.create();
  auto handle = state.assetRegistry.getAnimators().addAsset({});

  liquid::editor::EntityCreateAnimator action(entity, handle);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityCreateAnimatorActionTest);

using EntitySetAnimatorActionTest = ActionTestBase;

TEST_P(EntitySetAnimatorActionTest, ExecutorSetsAnimatorForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Animator>(entity, {});

  liquid::editor::EntitySetAnimator action(entity,
                                           liquid::AnimatorAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Animator>(entity).asset,
            liquid::AnimatorAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetAnimatorActionTest, UndoSetsPreviousAnimatorForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Animator>(
      entity, {liquid::AnimatorAssetHandle{25}});

  liquid::editor::EntitySetAnimator action(entity,
                                           liquid::AnimatorAssetHandle{15});
  action.onExecute(state);
  auto res = action.onUndo(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Animator>(entity).asset,
            liquid::AnimatorAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetAnimatorActionTest, PredicateReturnsFalseIfAnimatorIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntitySetAnimator action(entity,
                                           liquid::AnimatorAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntitySetAnimatorActionTest, PredicateReturnsTrueIfAnimatorExists) {
  auto entity = activeScene().entityDatabase.create();
  auto scriptHandle = state.assetRegistry.getAnimators().addAsset({});

  liquid::editor::EntitySetAnimator action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntitySetAnimatorActionTest);

using EntityDeleteAnimatorActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteAnimatorActionTest,
                                EntityDeleteAnimator, Animator);
InitActionsTestSuite(EntityActionsTest, EntityDeleteAnimatorActionTest);
