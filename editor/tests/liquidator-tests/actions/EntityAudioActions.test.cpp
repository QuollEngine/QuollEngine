#include "liquid/core/Base.h"
#include "liquidator/actions/EntityAudioActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetAudioActionTest = ActionTestBase;

TEST_F(EntitySetAudioActionTest, ExecutorSetsAudioSourceForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetAudio action(entity, liquid::AudioAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::AudioSource>(entity).source,
      liquid::AudioAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetAudioActionTest);
