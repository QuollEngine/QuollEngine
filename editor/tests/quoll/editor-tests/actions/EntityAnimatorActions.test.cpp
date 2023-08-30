#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityAnimatorActions.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateAnimatorActionTest = ActionTestBase;

TEST_P(EntityCreateAnimatorActionTest, ExecutorCreatesAnimatorSourceComponent) {
  auto entity = activeScene().entityDatabase.create();

  quoll::editor::EntityCreateAnimator action(entity,
                                             quoll::AnimatorAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Animator>(entity).asset,
            quoll::AnimatorAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityCreateAnimatorActionTest, UndoDeletesAnimatorSourceComponet) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Animator>(
      entity, {quoll::AnimatorAssetHandle{25}});

  quoll::editor::EntityCreateAnimator action(entity,
                                             quoll::AnimatorAssetHandle{15});
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Animator>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityCreateAnimatorActionTest,
       PredicateReturnsFalseIfAnimatorAssetIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateAnimator action(entity,
                                             quoll::AnimatorAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateAnimatorActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasAnimator) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateAnimator action(entity,
                                             quoll::AnimatorAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateAnimatorActionTest,
       PredicateReturnsTrueIfAnimatorDoesNotExistAndAssetIsValid) {
  auto entity = activeScene().entityDatabase.create();
  auto handle = assetRegistry.getAnimators().addAsset({});

  quoll::editor::EntityCreateAnimator action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntityCreateAnimatorActionTest);

using EntitySetAnimatorActionTest = ActionTestBase;

TEST_P(EntitySetAnimatorActionTest, ExecutorSetsAnimatorForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Animator>(entity, {});

  quoll::editor::EntitySetAnimator action(entity,
                                          quoll::AnimatorAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Animator>(entity).asset,
            quoll::AnimatorAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetAnimatorActionTest, UndoSetsPreviousAnimatorForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Animator>(
      entity, {quoll::AnimatorAssetHandle{25}});

  quoll::editor::EntitySetAnimator action(entity,
                                          quoll::AnimatorAssetHandle{15});
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Animator>(entity).asset,
            quoll::AnimatorAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetAnimatorActionTest, PredicateReturnsFalseIfAnimatorIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntitySetAnimator action(entity,
                                          quoll::AnimatorAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetAnimatorActionTest, PredicateReturnsTrueIfAnimatorExists) {
  auto entity = activeScene().entityDatabase.create();
  auto scriptHandle = assetRegistry.getAnimators().addAsset({});

  quoll::editor::EntitySetAnimator action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetAnimatorActionTest);

using EntityDeleteAnimatorActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteAnimatorActionTest,
                                EntityDeleteAnimator, Animator);
InitActionsTestSuite(EntityActionsTest, EntityDeleteAnimatorActionTest);
