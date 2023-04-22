#include "liquid/core/Base.h"
#include "liquidator/actions/EntityAudioActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateAudioActionTest = ActionTestBase;

TEST_P(EntityCreateAudioActionTest, ExecutorCreatesAudioSourceComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityCreateAudio action(entity,
                                           liquid::AudioAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::AudioSource>(entity).source,
      liquid::AudioAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityCreateAudioActionTest, UndoDeletesAudioSourceComponet) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::AudioSource>(
      entity, {liquid::AudioAssetHandle{25}});

  liquid::editor::EntityCreateAudio action(entity,
                                           liquid::AudioAssetHandle{15});
  auto res = action.onUndo(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::AudioSource>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityCreateAudioActionTest,
       PredicateReturnsFalseIfAudioAssetIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntityCreateAudio action(entity,
                                           liquid::AudioAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntityCreateAudioActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasAudio) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntityCreateAudio action(entity,
                                           liquid::AudioAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntityCreateAudioActionTest,
       PredicateReturnsTrueIfAudioDoesNotExistAndAssetIsValid) {
  auto entity = activeScene().entityDatabase.create();
  auto handle = state.assetRegistry.getAudios().addAsset({});

  liquid::editor::EntityCreateAudio action(entity, handle);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityCreateAudioActionTest);

using EntitySetAudioActionTest = ActionTestBase;

TEST_P(EntitySetAudioActionTest, ExecutorSetsAudioForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::AudioSource>(entity, {});

  liquid::editor::EntitySetAudio action(entity, liquid::AudioAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::AudioSource>(entity).source,
      liquid::AudioAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetAudioActionTest, UndoSetsPreviousAudioForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::AudioSource>(
      entity, {liquid::AudioAssetHandle{25}});

  liquid::editor::EntitySetAudio action(entity, liquid::AudioAssetHandle{15});
  action.onExecute(state);
  auto res = action.onUndo(state);

  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::AudioSource>(entity).source,
      liquid::AudioAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetAudioActionTest, PredicateReturnsFalseIfAudioIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  liquid::editor::EntitySetAudio action(entity, liquid::AudioAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(EntitySetAudioActionTest, PredicateReturnsTrueIfAudioExists) {
  auto entity = activeScene().entityDatabase.create();
  auto scriptHandle = state.assetRegistry.getAudios().addAsset({});

  liquid::editor::EntitySetAudio action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntitySetAudioActionTest);

using EntityDeleteAudioActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteAudioActionTest, EntityDeleteAudio,
                                AudioSource);
InitActionsTestSuite(EntityActionsTest, EntityDeleteAudioActionTest);
