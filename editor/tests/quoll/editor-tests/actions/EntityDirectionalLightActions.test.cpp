#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityLightActions.h"

#include "quoll/editor-tests/Testing.h"
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
  activeScene().entityDatabase.set<quoll::DirectionalLight>(entity, {});

  quoll::editor::EntityDeleteDirectionalLight action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(
      activeScene().entityDatabase.has<quoll::CascadedShadowMap>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteDirectionalLightActionTest,
       UndoCreatesCascadedShadowMapForEntityIfItExistedDuringExecution) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::DirectionalLight>(entity, {});
  activeScene().entityDatabase.set<quoll::CascadedShadowMap>(entity, {});

  quoll::editor::EntityDeleteDirectionalLight action(entity);
  action.onExecute(state, assetRegistry);

  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(
      activeScene().entityDatabase.has<quoll::CascadedShadowMap>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteDirectionalLightActionTest);
