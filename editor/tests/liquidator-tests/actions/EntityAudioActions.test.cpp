#include "liquid/core/Base.h"
#include "liquidator/actions/EntityAudioActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetAudioActionTest = ActionTestBase;

TEST_F(EntitySetAudioActionTest, ExecutorSetsAudioSourceForEntityInScene) {
  auto entity = state.scene.entityDatabase.create();

  liquid::editor::EntitySetAudio action(entity, liquid::AudioAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(state.scene.entityDatabase.get<liquid::AudioSource>(entity).source,
            liquid::AudioAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetAudioActionTest, ExecutorSetsScriptForEntityInSimulationScene) {
  state.mode = liquid::editor::WorkspaceMode::Simulation;

  auto entity = state.simulationScene.entityDatabase.create();

  liquid::editor::EntitySetAudio action(entity, liquid::AudioAssetHandle{15});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      state.simulationScene.entityDatabase.get<liquid::AudioSource>(entity)
          .source,
      liquid::AudioAssetHandle{15});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
