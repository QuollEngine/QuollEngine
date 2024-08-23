#include "quoll/core/Base.h"
#include "quoll/scene/Sprite.h"
#include "quoll/editor/actions/EntitySpriteActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateSpriteActionTest = ActionTestBase;

TEST_F(EntityCreateSpriteActionTest, ExecutorCreatesSpriteComponent) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::EntityCreateSprite action(entity,
                                           quoll::TextureAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(entity.get_ref<quoll::Sprite>()->handle,
            quoll::TextureAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityCreateSpriteActionTest, UndoDeletesSpriteComponent) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::Sprite>({quoll::TextureAssetHandle{25}});

  quoll::editor::EntityCreateSprite action(entity,
                                           quoll::TextureAssetHandle{15});
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::Sprite>());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityCreateSpriteActionTest,
       PredicateReturnsFalseIfTextureAssetIsInvalid) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityCreateSprite action(entity,
                                           quoll::TextureAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityCreateSpriteActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasSprite) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityCreateSprite action(entity,
                                           quoll::TextureAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityCreateSpriteActionTest,
       PredicateReturnsTrueIfSpriteDoesNotExistAndAssetIsValid) {
  auto entity = state.scene.entityDatabase.entity();
  auto handle = assetRegistry.getTextures().addAsset({});

  quoll::editor::EntityCreateSprite action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

using EntitySetSpriteActionTest = ActionTestBase;

TEST_F(EntitySetSpriteActionTest, ExecutorSetsSpriteForEntity) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::Sprite>({});

  quoll::editor::EntitySetSprite action(entity, quoll::TextureAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(entity.get_ref<quoll::Sprite>()->handle,
            quoll::TextureAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetSpriteActionTest, UndoSetsPreviousSpriteForEntity) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::Sprite>({quoll::TextureAssetHandle{25}});

  quoll::editor::EntitySetSprite action(entity, quoll::TextureAssetHandle{15});
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(entity.get_ref<quoll::Sprite>()->handle,
            quoll::TextureAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetSpriteActionTest, PredicateReturnsFalseIfTextureIsInvalid) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntitySetSprite action(entity, quoll::TextureAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntitySetSpriteActionTest, PredicateReturnsTrueIfSpriteExists) {
  auto entity = state.scene.entityDatabase.entity();
  auto handle = assetRegistry.getTextures().addAsset({});

  quoll::editor::EntitySetSprite action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}
