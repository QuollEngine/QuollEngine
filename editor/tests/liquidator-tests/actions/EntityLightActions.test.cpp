#include "liquid/core/Base.h"
#include "liquidator/actions/EntityLightActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetDirectionalLightActionTest = ActionTestBase;

TEST_P(EntitySetDirectionalLightActionTest, SetsDirectionalLightToEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetDirectionalLight action(entity, {glm::vec4{0.3f}});
  auto res = action.onExecute(state);

  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::DirectionalLight>(entity).color,
      glm::vec4(0.3f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetDirectionalLightActionTest);

using EntityEnableCascadedShadowMapActionTest = ActionTestBase;

TEST_P(EntityEnableCascadedShadowMapActionTest,
       ExecutorCreatesCascadedShadowMapComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityEnableCascadedShadowMap action(entity);
  auto res = action.onExecute(state);

  EXPECT_TRUE(
      activeScene().entityDatabase.has<liquid::CascadedShadowMap>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityEnableCascadedShadowMapActionTest,
       PredicateReturnsTrueIfEntityDoesNotHaveCascadedShadowMapComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityEnableCascadedShadowMap action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntityEnableCascadedShadowMapActionTest,
       PredicateReturnsFalseIfEntityHasCascadedShadowMapComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::CascadedShadowMap>(entity, {});

  liquid::editor::EntityEnableCascadedShadowMap action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest,
                     EntityEnableCascadedShadowMapActionTest);

using EntityDisableCascadedShadowMapActionTest = ActionTestBase;

TEST_P(EntityDisableCascadedShadowMapActionTest,
       ExecutorRemovesCascadedShadowMapComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::CascadedShadowMap>(entity, {});

  liquid::editor::EntityDisableCascadedShadowMap action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::CascadedShadowMap>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDisableCascadedShadowMapActionTest,
       PredicateReturnsTrueIfEntityHasCascadedShadowMapComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::CascadedShadowMap>(entity, {});

  liquid::editor::EntityDisableCascadedShadowMap action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntityDisableCascadedShadowMapActionTest,
       PredicateReturnsFalseIfEntityDoesNotHaveCascadedShadowMapComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityDisableCascadedShadowMap action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest,
                     EntityDisableCascadedShadowMapActionTest);

using EntitySetCascadedShadowMapActionTest = ActionTestBase;

TEST_P(EntitySetCascadedShadowMapActionTest, ExecutorSetsCascadedShadowMap) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetCascadedShadowMapAction action(entity, {0.4f});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::CascadedShadowMap>(entity)
                .splitLambda,
            0.4f);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetCascadedShadowMapActionTest);

using EntitySetPointLightActionTest = ActionTestBase;

TEST_P(EntitySetPointLightActionTest, ExecutorSetsPointLightForEntity) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetPointLight action(entity, {glm::vec4{0.3f}});
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::PointLight>(entity).color,
            glm::vec4(0.3f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetPointLightActionTest, ExecutorRemovesOtherLightsOnEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::DirectionalLight>(entity, {});

  liquid::editor::EntitySetPointLight action(entity, {glm::vec4{0.3f}});
  auto res = action.onExecute(state);

  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::DirectionalLight>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::PointLight>(entity).color,
            glm::vec4(0.3f));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

InitActionsTestSuite(EntityActionsTest, EntitySetPointLightActionTest);
