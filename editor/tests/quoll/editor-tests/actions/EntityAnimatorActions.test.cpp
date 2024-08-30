#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityAnimatorActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateAnimatorActionTest = ActionTestBase;

TEST_F(EntityCreateAnimatorActionTest, ExecutorCreatesAnimatorSourceComponent) {
  auto entity = state.scene.entityDatabase.create();

  quoll::editor::EntityCreateAnimator action(
      entity, quoll::AssetHandle<quoll::AnimatorAsset>{15});
  auto res = action.onExecute(state, assetCache);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Animator>(entity).asset,
            quoll::AssetHandle<quoll::AnimatorAsset>{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityCreateAnimatorActionTest, UndoDeletesAnimatorSourceComponet) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Animator>(
      entity, {quoll::AssetHandle<quoll::AnimatorAsset>{25}});

  quoll::editor::EntityCreateAnimator action(
      entity, quoll::AssetHandle<quoll::AnimatorAsset>{15});
  auto res = action.onUndo(state, assetCache);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Animator>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityCreateAnimatorActionTest,
       PredicateReturnsFalseIfAnimatorAssetIsInvalid) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateAnimator action(
      entity, quoll::AssetHandle<quoll::AnimatorAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityCreateAnimatorActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasAnimator) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateAnimator action(
      entity, quoll::AssetHandle<quoll::AnimatorAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityCreateAnimatorActionTest,
       PredicateReturnsTrueIfAnimatorDoesNotExistAndAssetIsValid) {
  auto entity = state.scene.entityDatabase.create();
  auto handle = assetCache.getRegistry().add<quoll::AnimatorAsset>({});

  quoll::editor::EntityCreateAnimator action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetCache));
}

using EntitySetAnimatorActionTest = ActionTestBase;

TEST_F(EntitySetAnimatorActionTest, ExecutorSetsAnimatorForEntity) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Animator>(entity, {});

  quoll::editor::EntitySetAnimator action(
      entity, quoll::AssetHandle<quoll::AnimatorAsset>{15});
  auto res = action.onExecute(state, assetCache);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Animator>(entity).asset,
            quoll::AssetHandle<quoll::AnimatorAsset>{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetAnimatorActionTest, UndoSetsPreviousAnimatorForEntity) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Animator>(
      entity, {quoll::AssetHandle<quoll::AnimatorAsset>{25}});

  quoll::editor::EntitySetAnimator action(
      entity, quoll::AssetHandle<quoll::AnimatorAsset>{15});
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Animator>(entity).asset,
            quoll::AssetHandle<quoll::AnimatorAsset>{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetAnimatorActionTest, PredicateReturnsFalseIfAnimatorIsInvalid) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntitySetAnimator action(
      entity, quoll::AssetHandle<quoll::AnimatorAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetAnimatorActionTest, PredicateReturnsTrueIfAnimatorExists) {
  auto entity = state.scene.entityDatabase.create();
  auto handle = assetCache.getRegistry().add<quoll::AnimatorAsset>({});

  quoll::editor::EntitySetAnimator action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetCache));
}
