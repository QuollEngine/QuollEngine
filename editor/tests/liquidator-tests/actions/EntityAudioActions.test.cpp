#include "quoll/core/Base.h"
#include "liquidator/actions/EntityAudioActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateAudioActionTest = ActionTestBase;

TEST_P(EntityCreateAudioActionTest, ExecutorCreatesAudioSourceComponent) {
  auto entity = activeScene().entityDatabase.create();

  quoll::editor::EntityCreateAudio action(entity, quoll::AudioAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::AudioSource>(entity).source,
            quoll::AudioAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityCreateAudioActionTest, UndoDeletesAudioSourceComponet) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::AudioSource>(
      entity, {quoll::AudioAssetHandle{25}});

  quoll::editor::EntityCreateAudio action(entity, quoll::AudioAssetHandle{15});
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::AudioSource>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityCreateAudioActionTest,
       PredicateReturnsFalseIfAudioAssetIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateAudio action(entity, quoll::AudioAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateAudioActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasAudio) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntityCreateAudio action(entity, quoll::AudioAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntityCreateAudioActionTest,
       PredicateReturnsTrueIfAudioDoesNotExistAndAssetIsValid) {
  auto entity = activeScene().entityDatabase.create();
  auto handle = assetRegistry.getAudios().addAsset({});

  quoll::editor::EntityCreateAudio action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntityCreateAudioActionTest);

using EntitySetAudioActionTest = ActionTestBase;

TEST_P(EntitySetAudioActionTest, ExecutorSetsAudioForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::AudioSource>(entity, {});

  quoll::editor::EntitySetAudio action(entity, quoll::AudioAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::AudioSource>(entity).source,
            quoll::AudioAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetAudioActionTest, UndoSetsPreviousAudioForEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::AudioSource>(
      entity, {quoll::AudioAssetHandle{25}});

  quoll::editor::EntitySetAudio action(entity, quoll::AudioAssetHandle{15});
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(activeScene().entityDatabase.get<quoll::AudioSource>(entity).source,
            quoll::AudioAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetAudioActionTest, PredicateReturnsFalseIfAudioIsInvalid) {
  auto entity = activeScene().entityDatabase.create();
  quoll::editor::EntitySetAudio action(entity, quoll::AudioAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(EntitySetAudioActionTest, PredicateReturnsTrueIfAudioExists) {
  auto entity = activeScene().entityDatabase.create();
  auto scriptHandle = assetRegistry.getAudios().addAsset({});

  quoll::editor::EntitySetAudio action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetAudioActionTest);

using EntityDeleteAudioActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDeleteAudioActionTest, EntityDeleteAudio,
                                AudioSource);
InitActionsTestSuite(EntityActionsTest, EntityDeleteAudioActionTest);
