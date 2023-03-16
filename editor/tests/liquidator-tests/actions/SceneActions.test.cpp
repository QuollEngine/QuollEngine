#include "liquid/core/Base.h"
#include "liquidator/actions/SceneActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using SceneSetStartingCameraActionTest = ActionTestBase;

TEST_P(SceneSetStartingCameraActionTest, ExecutorSetsActiveCamera) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::SceneSetStartingCamera action(entity);
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().activeCamera, entity);
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneSetStartingCameraActionTest,
       PredicateReturnsFalseIfProvidedEntityEqualsActiveCamera) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});
  activeScene().activeCamera = entity;

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(SceneSetStartingCameraActionTest,
       PredicateReturnsFalseIfProvidedEntityDoesNotHavePerspectiveLens) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(SceneSetStartingCameraActionTest,
       PredicateReturnsTrueIfProvidedCameraEntityIsDifferentThanActiveCamera) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::PerspectiveLens>(entity, {});

  liquid::editor::SceneSetStartingCamera action(entity);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(SceneActionsTest, SceneSetStartingCameraActionTest);

using SceneRemoveSkyboxActionTest = ActionTestBase;

TEST_P(SceneRemoveSkyboxActionTest,
       ExecutorRemovesSkyboxComponentFromEnvironmentEntity) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::EnvironmentSkybox>(
      activeScene().environment, {});

  liquid::editor::SceneRemoveSkybox action;
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::EnvironmentSkybox>(
      activeScene().environment));
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneRemoveSkyboxActionTest,
       PredicateReturnsFalseIfEntityDoesNotHaveSkybox) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::editor::SceneRemoveSkybox action;
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(SceneRemoveSkyboxActionTest, PredicateReturnsTrueIfEntityHasSkybox) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::EnvironmentSkybox>(
      activeScene().environment, {});

  liquid::editor::SceneRemoveSkybox action;
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(SceneActionsTest, SceneRemoveSkyboxActionTest);

using SceneSetSkyboxColorActionTest = ActionTestBase;

TEST_P(SceneSetSkyboxColorActionTest,
       ExecutorSetsSkyboxTextureForEnvironmentEntity) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::editor::SceneSetSkyboxColor action(glm::vec4{0.2f});
  auto res = action.onExecute(state);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::EnvironmentSkybox>(
      activeScene().environment));
  EXPECT_EQ(activeScene()
                .entityDatabase
                .get<liquid::EnvironmentSkybox>(activeScene().environment)
                .type,
            liquid::EnvironmentSkyboxType::Color);
  EXPECT_EQ(activeScene()
                .entityDatabase
                .get<liquid::EnvironmentSkybox>(activeScene().environment)
                .color,
            glm::vec4{0.2f});
  EXPECT_TRUE(res.saveScene);
}

InitActionsTestSuite(SceneActionsTest, SceneSetSkyboxColorActionTest);

using SceneSetSkyboxTextureActionTest = ActionTestBase;

TEST_P(SceneSetSkyboxTextureActionTest,
       ExecutorSetsSkyboxTextureForEnvironmentEntity) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::editor::SceneSetSkyboxTexture action(
      liquid::EnvironmentAssetHandle{20});
  auto res = action.onExecute(state);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::EnvironmentSkybox>(
      activeScene().environment));
  EXPECT_EQ(activeScene()
                .entityDatabase
                .get<liquid::EnvironmentSkybox>(activeScene().environment)
                .type,
            liquid::EnvironmentSkyboxType::Texture);
  EXPECT_EQ(activeScene()
                .entityDatabase
                .get<liquid::EnvironmentSkybox>(activeScene().environment)
                .texture,
            liquid::EnvironmentAssetHandle{20});
  EXPECT_TRUE(res.saveScene);
}

InitActionsTestSuite(SceneActionsTest, SceneSetSkyboxTextureActionTest);

using SceneRemoveLightingActionTest = ActionTestBase;

TEST_P(SceneRemoveLightingActionTest,
       ExecutorRemovesEnvironmentLightingComponen) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::EnvironmentLightingSkyboxSource>(
      activeScene().environment, {});

  liquid::editor::SceneRemoveLighting action;
  auto res = action.onExecute(state);

  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
          activeScene().environment));
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneRemoveLightingActionTest,
       PredicateReturnsTrueIfEnvironmentHasLightingSource) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::EnvironmentLightingSkyboxSource>(
      activeScene().environment, {});

  liquid::editor::SceneRemoveLighting action;
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(SceneRemoveLightingActionTest,
       PredicateReturnsFalseIfEnvironmentDoesNotHaveLightingSource) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::editor::SceneRemoveLighting action;
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(SceneActionsTest, SceneRemoveLightingActionTest);

using SceneSetSkyboxLightingSourceActionTest = ActionTestBase;

TEST_P(SceneSetSkyboxLightingSourceActionTest,
       ExecutorSetsSkyboxLightingSourceComponentForEnvironmentEntity) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::editor::SceneSetSkyboxLightingSource action;
  auto res = action.onExecute(state);

  EXPECT_TRUE(
      activeScene().entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
          activeScene().environment));
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneSetSkyboxLightingSourceActionTest,
       PredicateReturnsTrueIfEnvironmentDoesNotHaveSkyboxLightingSource) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::editor::SceneSetSkyboxLightingSource action;
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(SceneSetSkyboxLightingSourceActionTest,
       PredicateReturnsFalseIfEnvironmentDoesHasSkyboxLightingSource) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::EnvironmentLightingSkyboxSource>(
      activeScene().environment, {});

  liquid::editor::SceneSetSkyboxLightingSource action;
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(SceneActionsTest, SceneSetSkyboxLightingSourceActionTest);
