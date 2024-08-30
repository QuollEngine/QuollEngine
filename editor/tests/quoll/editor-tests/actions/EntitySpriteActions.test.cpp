#include "quoll/core/Base.h"
#include "quoll/scene/Sprite.h"
#include "quoll/editor/actions/EntitySpriteActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateSpriteActionTest = ActionTestBase;

TEST_F(EntityCreateSpriteActionTest, ExecutorCreatesSpriteComponent) {
  auto entity = state.scene.entityDatabase.create();

  quoll::editor::EntityCreateSprite action(
      entity, quoll::AssetHandle<quoll::TextureAsset>{15});
  auto res = action.onExecute(state, assetCache);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Sprite>(entity).handle,
            quoll::AssetHandle<quoll::TextureAsset>{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityCreateSpriteActionTest, UndoDeletesSpriteComponent) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Sprite>(
      entity, {quoll::AssetHandle<quoll::TextureAsset>{25}});

  quoll::editor::EntityCreateSprite action(
      entity, quoll::AssetHandle<quoll::TextureAsset>{15});
  auto res = action.onUndo(state, assetCache);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Sprite>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityCreateSpriteActionTest,
       PredicateReturnsFalseIfTextureAssetIsInvalid) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateSprite action(
      entity, quoll::AssetHandle<quoll::TextureAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityCreateSpriteActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasSprite) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateSprite action(
      entity, quoll::AssetHandle<quoll::TextureAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntityCreateSpriteActionTest,
       PredicateReturnsTrueIfSpriteDoesNotExistAndAssetIsValid) {
  auto entity = state.scene.entityDatabase.create();
  auto handle = assetCache.getRegistry().add<quoll::TextureAsset>({});

  quoll::editor::EntityCreateSprite action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetCache));
}

using EntitySetSpriteActionTest = ActionTestBase;

TEST_F(EntitySetSpriteActionTest, ExecutorSetsSpriteForEntity) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Sprite>(entity, {});

  quoll::editor::EntitySetSprite action(
      entity, quoll::AssetHandle<quoll::TextureAsset>{15});
  auto res = action.onExecute(state, assetCache);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Sprite>(entity).handle,
            quoll::AssetHandle<quoll::TextureAsset>{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetSpriteActionTest, UndoSetsPreviousSpriteForEntity) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::Sprite>(
      entity, {quoll::AssetHandle<quoll::TextureAsset>{25}});

  quoll::editor::EntitySetSprite action(
      entity, quoll::AssetHandle<quoll::TextureAsset>{15});
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Sprite>(entity).handle,
            quoll::AssetHandle<quoll::TextureAsset>{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetSpriteActionTest, PredicateReturnsFalseIfTextureIsInvalid) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntitySetSprite action(
      entity, quoll::AssetHandle<quoll::TextureAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetCache));
}

TEST_F(EntitySetSpriteActionTest, PredicateReturnsTrueIfSpriteExists) {
  auto entity = state.scene.entityDatabase.create();
  auto handle = assetCache.getRegistry().add<quoll::TextureAsset>({});

  quoll::editor::EntitySetSprite action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetCache));
}
