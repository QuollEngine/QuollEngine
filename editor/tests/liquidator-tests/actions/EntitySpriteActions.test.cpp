#include "liquid/core/Base.h"
#include "liquidator/actions/EntitySpriteActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateSpriteActionTest = ActionTestBase;

TEST_P(EntityCreateSpriteActionTest, ExecutorCreatesSpriteComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityCreateSprite action(entity,
                                            liquid::TextureAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Sprite>(entity).handle,
            liquid::TextureAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityCreateSpriteActionTest, UndoDeletesSpriteComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Sprite>(
      entity, {liquid::TextureAssetHandle{25}});

  liquid::editor::EntityCreateSprite action(entity,
                                            liquid::TextureAssetHandle{15});
  auto res = action.onUndo(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Sprite>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityCreateSpriteActionTest,
       PredicateReturnsFalseIfTextureAssetIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntityCreateSprite action(entity,
                                            liquid::TextureAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntityCreateSpriteActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasSprite) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntityCreateSprite action(entity,
                                            liquid::TextureAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntityCreateSpriteActionTest,
       PredicateReturnsTrueIfSpriteDoesNotExistAndAssetIsValid) {
  auto entity = activeScene().entityDatabase.create();
  auto handle = state.assetRegistry.getTextures().addAsset({});

  liquid::editor::EntityCreateSprite action(entity, handle);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityCreateSpriteActionTest);

using EntitySetSpriteActionTest = ActionTestBase;

TEST_P(EntitySetSpriteActionTest, ExecutorSetsSpriteForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Sprite>(entity, {});

  liquid::editor::EntitySetSprite action(entity,
                                         liquid::TextureAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Sprite>(entity).handle,
            liquid::TextureAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetSpriteActionTest, UndoSetsPreviousSpriteForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Sprite>(
      entity, {liquid::TextureAssetHandle{25}});

  liquid::editor::EntitySetSprite action(entity,
                                         liquid::TextureAssetHandle{15});
  action.onExecute(state);
  auto res = action.onUndo(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Sprite>(entity).handle,
            liquid::TextureAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetSpriteActionTest, PredicateReturnsFalseIfTextureIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntitySetSprite action(entity,
                                         liquid::TextureAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntitySetSpriteActionTest, PredicateReturnsTrueIfSpriteExists) {
  auto entity = activeScene().entityDatabase.create();
  auto handle = state.assetRegistry.getTextures().addAsset({});

  liquid::editor::EntitySetSprite action(entity, handle);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntitySetSpriteActionTest);

using EntityDeleteSpriteActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteSpriteActionTest,
                                EntityDeleteSprite, Sprite);
InitActionsTestSuite(EntityActionsTest, EntityDeleteSpriteActionTest);
