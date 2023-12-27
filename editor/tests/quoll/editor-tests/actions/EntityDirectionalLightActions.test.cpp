#include "quoll/core/Base.h"
#include "quoll/scene/DirectionalLight.h"
#include "quoll/scene/CascadedShadowMap.h"

#include "quoll/editor/actions/EntityLightActions.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

// Directional light
using EntityDeleteDirectionalLightActionTest = ActionTestBase;

InitDefaultDeleteComponentTests(EntityDeleteDirectionalLightActionTest,
                                EntityDeleteDirectionalLight, DirectionalLight);

TEST_F(
    EntityDeleteDirectionalLightActionTest,
    UndoDoesNotCreateCascadedShadowMapForEntityIfItDidNotExistDuringExecution) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::DirectionalLight>(entity, {});

  quoll::editor::EntityDeleteDirectionalLight action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(
      state.scene.entityDatabase.has<quoll::CascadedShadowMap>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityDeleteDirectionalLightActionTest,
       UndoCreatesCascadedShadowMapForEntityIfItExistedDuringExecution) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::DirectionalLight>(entity, {});
  state.scene.entityDatabase.set<quoll::CascadedShadowMap>(entity, {});

  quoll::editor::EntityDeleteDirectionalLight action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::CascadedShadowMap>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
