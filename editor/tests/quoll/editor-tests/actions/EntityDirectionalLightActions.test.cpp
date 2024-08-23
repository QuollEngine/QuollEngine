#include "quoll/core/Base.h"
#include "quoll/scene/CascadedShadowMap.h"
#include "quoll/scene/DirectionalLight.h"
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
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::DirectionalLight>({});

  quoll::editor::EntityDeleteDirectionalLight action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::CascadedShadowMap>());
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntityDeleteDirectionalLightActionTest,
       UndoCreatesCascadedShadowMapForEntityIfItExistedDuringExecution) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::DirectionalLight>({});
  entity.set<quoll::CascadedShadowMap>({});

  quoll::editor::EntityDeleteDirectionalLight action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(entity.has<quoll::CascadedShadowMap>());
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}
