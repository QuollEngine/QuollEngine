#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntitySpriteActions.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateSpriteActionTest = ActionTestBase;

TEST_P(EntityCreateSpriteActionTest, ExecutorCreatesSpriteComponent) {
  auto entity = activeScene().entityDatabase.create();

  quoll::editor::EntityCreateSprite action(entity,
                                           quoll::TextureAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Sprite>(entity).handle,
            quoll::TextureAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityCreateSpriteActionTest, UndoDeletesSpriteComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Sprite>(
      entity, {quoll::TextureAssetHandle{25}});

  quoll::editor::EntityCreateSprite action(entity,
                                           quoll::TextureAssetHandle{15});
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Sprite>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityCreateSpriteActionTest,
       PredicateReturnsFalseIfTextureAssetIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateSprite action(entity,
                                           quoll::TextureAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateSpriteActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasSprite) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateSprite action(entity,
                                           quoll::TextureAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateSpriteActionTest,
       PredicateReturnsTrueIfSpriteDoesNotExistAndAssetIsValid) {
  auto entity = activeScene().entityDatabase.create();
  auto handle = assetRegistry.getTextures().addAsset({});

  quoll::editor::EntityCreateSprite action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntityCreateSpriteActionTest);

using EntitySetSpriteActionTest = ActionTestBase;

TEST_P(EntitySetSpriteActionTest, ExecutorSetsSpriteForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Sprite>(entity, {});

  quoll::editor::EntitySetSprite action(entity, quoll::TextureAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Sprite>(entity).handle,
            quoll::TextureAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetSpriteActionTest, UndoSetsPreviousSpriteForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Sprite>(
      entity, {quoll::TextureAssetHandle{25}});

  quoll::editor::EntitySetSprite action(entity, quoll::TextureAssetHandle{15});
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Sprite>(entity).handle,
            quoll::TextureAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetSpriteActionTest, PredicateReturnsFalseIfTextureIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntitySetSprite action(entity, quoll::TextureAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetSpriteActionTest, PredicateReturnsTrueIfSpriteExists) {
  auto entity = activeScene().entityDatabase.create();
  auto handle = assetRegistry.getTextures().addAsset({});

  quoll::editor::EntitySetSprite action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetSpriteActionTest);

using EntityDeleteSpriteActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteSpriteActionTest,
                                EntityDeleteSprite, Sprite);
InitActionsTestSuite(EntityActionsTest, EntityDeleteSpriteActionTest);
