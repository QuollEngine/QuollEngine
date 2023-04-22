#include "liquid/core/Base.h"
#include "liquidator/actions/EntityLightActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

// Directional light
using EntityCreateDirectionalLightActionTest = ActionTestBase;
InitDefaultCreateComponentTests(EntityCreateDirectionalLightActionTest,
                                EntityCreateDirectionalLight, DirectionalLight);
InitActionsTestSuite(EntityActionsTest, EntityCreateDirectionalLightActionTest);

using EntitySetDirectionalLightActionTest = ActionTestBase;
InitDefaultUpdateComponentTests(EntitySetDirectionalLightActionTest,
                                EntitySetDirectionalLight, DirectionalLight,
                                color, glm::vec4{2.5f});
InitActionsTestSuite(EntityActionsTest, EntitySetDirectionalLightActionTest);

// Shadow maps
using EntityEnableCascadedShadowMapsActionTest = ActionTestBase;
InitDefaultCreateComponentTests(EntityEnableCascadedShadowMapsActionTest,
                                EntityEnableCascadedShadowMap,
                                CascadedShadowMap);
InitActionsTestSuite(EntityActionsTest,
                     EntityEnableCascadedShadowMapsActionTest);

using EntityDisableCascadedShadowMapActionTest = ActionTestBase;
InitDefaultDeleteComponentTests(EntityDisableCascadedShadowMapActionTest,
                                EntityDisableCascadedShadowMap,
                                CascadedShadowMap);
InitActionsTestSuite(EntityActionsTest,
                     EntityDisableCascadedShadowMapActionTest);

using EntitySetCascadedShadowMapActionTest = ActionTestBase;

InitDefaultUpdateComponentTests(EntitySetCascadedShadowMapActionTest,
                                EntitySetCascadedShadowMap, CascadedShadowMap,
                                splitLambda, 1.2f);

InitActionsTestSuite(EntityActionsTest, EntitySetCascadedShadowMapActionTest);

using EntityDeleteDirectionalLightActionTest = ActionTestBase;

InitDefaultDeleteComponentTests(EntityDeleteDirectionalLightActionTest,
                                EntityDeleteDirectionalLight, DirectionalLight);

TEST_P(
    EntityDeleteDirectionalLightActionTest,
    UndoDoesNotCreateCascadedShadowMapForEntityIfItDidNotExistDuringExecution) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::DirectionalLight>(entity, {});

  liquid::editor::EntityDeleteDirectionalLight action(entity);
  action.onExecute(state);
  auto res = action.onUndo(state);

  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::CascadedShadowMap>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteDirectionalLightActionTest,
       UndoCreatesCascadedShadowMapForEntityIfItExistedDuringExecution) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::DirectionalLight>(entity, {});
  activeScene().entityDatabase.set<liquid::CascadedShadowMap>(entity, {});

  liquid::editor::EntityDeleteDirectionalLight action(entity);
  action.onExecute(state);

  auto res = action.onUndo(state);

  EXPECT_TRUE(
      activeScene().entityDatabase.has<liquid::CascadedShadowMap>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteDirectionalLightActionTest);
