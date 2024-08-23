#include "quoll/core/Base.h"
#include "quoll/audio/AudioSource.h"
#include "quoll/editor/actions/EntityAudioActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

using EntityCreateAudioActionTest = ActionTestBase;

TEST_F(EntityCreateAudioActionTest, ExecutorCreatesAudioSourceComponent) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::editor::EntityCreateAudio action(entity, quoll::AudioAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(entity.get_ref<quoll::AudioSource>()->source,
            quoll::AudioAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityCreateAudioActionTest, UndoDeletesAudioSourceComponet) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::AudioSource>({quoll::AudioAssetHandle{25}});

  quoll::editor::EntityCreateAudio action(entity, quoll::AudioAssetHandle{15});
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::AudioSource>());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityCreateAudioActionTest,
       PredicateReturnsFalseIfAudioAssetIsInvalid) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityCreateAudio action(entity, quoll::AudioAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityCreateAudioActionTest,
       PredicateReturnsFalseIfEntityAlreadyHasAudio) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntityCreateAudio action(entity, quoll::AudioAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntityCreateAudioActionTest,
       PredicateReturnsTrueIfAudioDoesNotExistAndAssetIsValid) {
  auto entity = state.scene.entityDatabase.entity();
  auto handle = assetRegistry.getAudios().addAsset({});

  quoll::editor::EntityCreateAudio action(entity, handle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

using EntitySetAudioActionTest = ActionTestBase;

TEST_F(EntitySetAudioActionTest, ExecutorSetsAudioForEntity) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::AudioSource>({});

  quoll::editor::EntitySetAudio action(entity, quoll::AudioAssetHandle{15});
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_EQ(entity.get_ref<quoll::AudioSource>()->source,
            quoll::AudioAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetAudioActionTest, UndoSetsPreviousAudioForEntity) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::AudioSource>({quoll::AudioAssetHandle{25}});

  quoll::editor::EntitySetAudio action(entity, quoll::AudioAssetHandle{15});
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_EQ(entity.get_ref<quoll::AudioSource>()->source,
            quoll::AudioAssetHandle{25});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetAudioActionTest, PredicateReturnsFalseIfAudioIsInvalid) {
  auto entity = state.scene.entityDatabase.entity();
  quoll::editor::EntitySetAudio action(entity, quoll::AudioAssetHandle{15});
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_F(EntitySetAudioActionTest, PredicateReturnsTrueIfAudioExists) {
  auto entity = state.scene.entityDatabase.entity();
  auto scriptHandle = assetRegistry.getAudios().addAsset({});

  quoll::editor::EntitySetAudio action(entity, scriptHandle);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}
