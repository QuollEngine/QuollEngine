#include "liquid/core/Base.h"
#include "liquidator/actions/EntityAudioActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetAudioActionTest = ActionTestBase;

TEST_P(EntitySetAudioActionTest, ExecutorSetsAudioSourceForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetAudio action(entity, liquid::AudioAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::AudioSource>(entity).source,
      liquid::AudioAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetAudioActionTest);

using EntityDeleteAudioActionTest = ActionTestBase;

TEST_P(EntityDeleteAudioActionTest, ExecutorDeletesAudioComponentFromEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::AudioSource>(entity, {});

  liquid::editor::EntityDeleteAudio action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::AudioSource>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteAudioActionTest,
       PredicateReturnsTrueIfEntityHasAudioSourceComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::AudioSource>(entity, {});

  liquid::editor::EntityDeleteAudio action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntityDeleteAudioActionTest,
       PredicateReturnsTrueIfEntityHasNoAudioSourceComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityDeleteAudio action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteAudioActionTest);
