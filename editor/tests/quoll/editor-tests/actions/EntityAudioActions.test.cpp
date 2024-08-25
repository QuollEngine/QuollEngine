#include "quoll/core/Base.h"
#include "quoll/audio/AudioSource.h"
#include "quoll/editor/actions/EntityAudioActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateAudioActionTest = ActionTestBase;

TEST_F(EntityCreateAudioActionTest, ExecutorCreatesAudioSourceComponent) {
  auto entity = state.scene.entityDatabase.create();

  quoll::editor::EntityCreateAudio action(
      entity, quoll::AssetHandle<quoll::AudioAsset>{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::AudioSource>(entity).source,
            quoll::AssetHandle<quoll::AudioAsset>{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityCreateAudioActionTest, UndoDeletesAudioSourceComponet) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::AudioSource>(
      entity, {quoll::AssetHandle<quoll::AudioAsset>{25}});

  quoll::editor::EntityCreateAudio action(
      entity, quoll::AssetHandle<quoll::AudioAsset>{15});
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::AudioSource>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityCreateAudioActionTest,
       PredicateReturnsFalseIfAudioAssetIsInvalid) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateAudio action(
      entity, quoll::AssetHandle<quoll::AudioAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityCreateAudioActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasAudio) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntityCreateAudio action(
      entity, quoll::AssetHandle<quoll::AudioAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityCreateAudioActionTest,
       PredicateReturnsTrueIfAudioDoesNotExistAndAssetIsValid) {
  auto entity = state.scene.entityDatabase.create();
  auto handle = assetRegistry.getAudios().addAsset({});

  quoll::editor::EntityCreateAudio action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

using EntitySetAudioActionTest = ActionTestBase;

TEST_F(EntitySetAudioActionTest, ExecutorSetsAudioForEntity) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::AudioSource>(entity, {});

  quoll::editor::EntitySetAudio action(
      entity, quoll::AssetHandle<quoll::AudioAsset>{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::AudioSource>(entity).source,
            quoll::AssetHandle<quoll::AudioAsset>{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetAudioActionTest, UndoSetsPreviousAudioForEntity) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::AudioSource>(
      entity, {quoll::AssetHandle<quoll::AudioAsset>{25}});

  quoll::editor::EntitySetAudio action(
      entity, quoll::AssetHandle<quoll::AudioAsset>{15});
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(state.scene.entityDatabase.get<quoll::AudioSource>(entity).source,
            quoll::AssetHandle<quoll::AudioAsset>{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetAudioActionTest, PredicateReturnsFalseIfAudioIsInvalid) {
  auto entity = state.scene.entityDatabase.create();
  quoll::editor::EntitySetAudio action(
      entity, quoll::AssetHandle<quoll::AudioAsset>{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntitySetAudioActionTest, PredicateReturnsTrueIfAudioExists) {
  auto entity = state.scene.entityDatabase.create();
  auto scriptHandle = assetRegistry.getAudios().addAsset({});

  quoll::editor::EntitySetAudio action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}
