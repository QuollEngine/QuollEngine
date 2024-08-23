#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityAnimatorActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateAnimatorActionTest = ActionTestBase;

TEST_F(EntityCreateAnimatorActionTest, ExecutorCreatesAnimatorSourceComponent) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::EntityCreateAnimator action(entity,
                                             quoll::AnimatorAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(entity.get_ref<quoll::Animator>()->asset,
            quoll::AnimatorAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityCreateAnimatorActionTest, UndoDeletesAnimatorSourceComponet) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::Animator>({quoll::AnimatorAssetHandle{25}});

  quoll::editor::EntityCreateAnimator action(entity,
                                             quoll::AnimatorAssetHandle{15});
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::Animator>());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityCreateAnimatorActionTest,
       PredicateReturnsFalseIfAnimatorAssetIsInvalid) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityCreateAnimator action(entity,
                                             quoll::AnimatorAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityCreateAnimatorActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasAnimator) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityCreateAnimator action(entity,
                                             quoll::AnimatorAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityCreateAnimatorActionTest,
       PredicateReturnsTrueIfAnimatorDoesNotExistAndAssetIsValid) {
  auto entity = state.scene.entityDatabase.entity();
  auto handle = assetRegistry.getAnimators().addAsset({});

  quoll::editor::EntityCreateAnimator action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

using EntitySetAnimatorActionTest = ActionTestBase;

TEST_F(EntitySetAnimatorActionTest, ExecutorSetsAnimatorForEntity) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::Animator>({});

  quoll::editor::EntitySetAnimator action(entity,
                                          quoll::AnimatorAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(entity.get_ref<quoll::Animator>()->asset,
            quoll::AnimatorAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetAnimatorActionTest, UndoSetsPreviousAnimatorForEntity) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::Animator>({quoll::AnimatorAssetHandle{25}});

  quoll::editor::EntitySetAnimator action(entity,
                                          quoll::AnimatorAssetHandle{15});
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(entity.get_ref<quoll::Animator>()->asset,
            quoll::AnimatorAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetAnimatorActionTest, PredicateReturnsFalseIfAnimatorIsInvalid) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntitySetAnimator action(entity,
                                          quoll::AnimatorAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntitySetAnimatorActionTest, PredicateReturnsTrueIfAnimatorExists) {
  auto entity = state.scene.entityDatabase.entity();
  auto scriptHandle = assetRegistry.getAnimators().addAsset({});

  quoll::editor::EntitySetAnimator action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}
