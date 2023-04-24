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
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SceneSetStartingCameraActionTest, UndoSetsPreviousActiveCamera) {
  auto entity = activeScene().entityDatabase.create();

  auto cameraEntity = activeScene().entityDatabase.create();
  activeScene().activeCamera = cameraEntity;

  liquid::editor::SceneSetStartingCamera action(entity);
  action.onExecute(state);

  auto res = action.onUndo(state);

  EXPECT_EQ(activeScene().activeCamera, cameraEntity);
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
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SceneRemoveSkyboxActionTest,
       UndoAddsPreviousSkyboxComponentForEnvironmentEntity) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::EnvironmentSkybox>(
      activeScene().environment, {});

  liquid::editor::SceneRemoveSkybox action;
  action.onExecute(state);

  auto res = action.onUndo(state);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::EnvironmentSkybox>(
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

using SceneChangeSkyboxTypeActionTest = ActionTestBase;

TEST_P(SceneChangeSkyboxTypeActionTest, ExecutorSetsDefaultSkybox) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::EnvironmentSkybox>(
      activeScene().environment, {});

  liquid::editor::SceneChangeSkyboxType action(
      liquid::EnvironmentSkyboxType::Texture);
  auto res = action.onExecute(state);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::EnvironmentSkybox>(
      activeScene().environment));
  EXPECT_EQ(activeScene()
                .entityDatabase
                .get<liquid::EnvironmentSkybox>(activeScene().environment)
                .type,
            liquid::EnvironmentSkyboxType::Texture);
  EXPECT_TRUE(res.saveScene);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SceneChangeSkyboxTypeActionTest,
       UndoSetsPreviousSkyboxForEnvironmentIfEnvironmentHadSkybox) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::EnvironmentSkybox>(
      activeScene().environment, {liquid::EnvironmentSkyboxType::Color});

  liquid::editor::SceneChangeSkyboxType action(
      liquid::EnvironmentSkyboxType::Texture);
  action.onExecute(state);

  auto res = action.onUndo(state);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::EnvironmentSkybox>(
      activeScene().environment));
  EXPECT_EQ(activeScene()
                .entityDatabase
                .get<liquid::EnvironmentSkybox>(activeScene().environment)
                .type,
            liquid::EnvironmentSkyboxType::Color);
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneChangeSkyboxTypeActionTest,
       UndoRemovesSkyboxFromEnvironmentIfEnvironmentDidNotHaveSkybox) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::editor::SceneChangeSkyboxType action(
      liquid::EnvironmentSkyboxType::Texture);
  action.onExecute(state);

  auto res = action.onUndo(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::EnvironmentSkybox>(
      activeScene().environment));
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneChangeSkyboxTypeActionTest,
       PredicateReturnsFalseIfTypeEqualsEnvironmentSkyboxType) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set(
      activeScene().environment,
      liquid::EnvironmentSkybox{liquid::EnvironmentSkyboxType::Texture});

  liquid::editor::SceneChangeSkyboxType action(
      liquid::EnvironmentSkyboxType::Texture);
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(SceneChangeSkyboxTypeActionTest,
       PredicateReturnsTrueIfEntityDoesNotHaveSkybox) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::editor::SceneChangeSkyboxType action(
      liquid::EnvironmentSkyboxType::Color);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(SceneChangeSkyboxTypeActionTest,
       PredicateReturnsTrueIfTypeDoesNotEqualEnvironmentSkyboxType) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set(
      activeScene().environment,
      liquid::EnvironmentSkybox{liquid::EnvironmentSkyboxType::Texture});

  liquid::editor::SceneChangeSkyboxType action(
      liquid::EnvironmentSkyboxType::Color);
  EXPECT_TRUE(action.predicate(state));
}

InitActionsTestSuite(SceneActionsTest, SceneChangeSkyboxTypeActionTest);

using SceneSetSkyboxColorActionTest = ActionTestBase;

TEST_P(SceneSetSkyboxColorActionTest, ExecutorSetsSkyboxColorForEnvironment) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set(activeScene().environment,
                                   liquid::EnvironmentSkybox{});

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
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SceneSetSkyboxColorActionTest,
       UndoSetsPreviousSkyboxColorForEnvironment) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::EnvironmentSkybox skybox{liquid::EnvironmentSkyboxType::Color};
  skybox.color = glm::vec4(0.6f);
  activeScene().entityDatabase.set(activeScene().environment, skybox);

  liquid::editor::SceneSetSkyboxColor action(glm::vec4{0.2f});
  action.onExecute(state);

  auto res = action.onUndo(state);

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
            glm::vec4{0.6f});
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneSetSkyboxColorActionTest,
       PredicateReturnsFalseIfEnvironmentDoesNotHaveSkybox) {
  activeScene().environment = activeScene().entityDatabase.create();
  EXPECT_FALSE(liquid::editor::SceneSetSkyboxColor({}).predicate(state));
}

TEST_P(SceneSetSkyboxColorActionTest,
       PredicateReturnsFalseIfEnvironmentSkyboxTypeIsNotColor) {
  activeScene().environment = activeScene().entityDatabase.create();
  liquid::EnvironmentSkybox skybox{liquid::EnvironmentSkyboxType::Texture};
  activeScene().entityDatabase.set(activeScene().environment, skybox);

  EXPECT_FALSE(liquid::editor::SceneSetSkyboxColor({}).predicate(state));
}

TEST_P(SceneSetSkyboxColorActionTest,
       PredicateReturnsTrueIfEnvironmentSkyboxTypeIsColor) {
  activeScene().environment = activeScene().entityDatabase.create();
  liquid::EnvironmentSkybox skybox{liquid::EnvironmentSkyboxType::Color};
  activeScene().entityDatabase.set(activeScene().environment, skybox);

  EXPECT_TRUE(liquid::editor::SceneSetSkyboxColor({}).predicate(state));
}

InitActionsTestSuite(SceneActionsTest, SceneSetSkyboxColorActionTest);

using SceneSetSkyboxTextureActionTest = ActionTestBase;

TEST_P(SceneSetSkyboxTextureActionTest,
       ExecutorSetsSkyboxTextureForEnvironmentEntity) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set(activeScene().environment,
                                   liquid::EnvironmentSkybox{});

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
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SceneSetSkyboxTextureActionTest,
       UndoSetsPreviousSkyboxTextureForEnvironment) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::EnvironmentSkybox skybox{liquid::EnvironmentSkyboxType::Texture};
  skybox.texture = liquid::EnvironmentAssetHandle{15};
  activeScene().entityDatabase.set(activeScene().environment, skybox);

  liquid::editor::SceneSetSkyboxTexture action(
      liquid::EnvironmentAssetHandle{20});
  action.onExecute(state);

  auto res = action.onUndo(state);

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
            liquid::EnvironmentAssetHandle{15});
  EXPECT_TRUE(res.saveScene);
}

TEST_P(SceneSetSkyboxTextureActionTest,
       PredicateReturnsFalseIfEnvironmentDoesNotHaveSkybox) {
  activeScene().environment = activeScene().entityDatabase.create();
  EXPECT_FALSE(liquid::editor::SceneSetSkyboxTexture({}).predicate(state));
}

TEST_P(SceneSetSkyboxTextureActionTest,
       PredicateReturnsFalseIfEnvironmentSkyboxTypeIsNotTexture) {
  activeScene().environment = activeScene().entityDatabase.create();
  liquid::EnvironmentSkybox skybox{liquid::EnvironmentSkyboxType::Color};
  activeScene().entityDatabase.set(activeScene().environment, skybox);

  EXPECT_FALSE(liquid::editor::SceneSetSkyboxTexture({}).predicate(state));
}

TEST_P(SceneSetSkyboxTextureActionTest,
       PredicateReturnsTrueIfEnvironmentSkyboxTypeIsTexture) {
  activeScene().environment = activeScene().entityDatabase.create();
  liquid::EnvironmentSkybox skybox{liquid::EnvironmentSkyboxType::Texture};
  activeScene().entityDatabase.set(activeScene().environment, skybox);

  EXPECT_TRUE(liquid::editor::SceneSetSkyboxTexture({}).predicate(state));
}

InitActionsTestSuite(SceneActionsTest, SceneSetSkyboxTextureActionTest);

using SceneRemoveLightingActionTest = ActionTestBase;

TEST_P(SceneRemoveLightingActionTest,
       ExecutorRemovesEnvironmentLightingComponent) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::EnvironmentLightingSkyboxSource>(
      activeScene().environment, {});

  liquid::editor::SceneRemoveLighting action;
  auto res = action.onExecute(state);

  EXPECT_FALSE(
      activeScene().entityDatabase.has<liquid::EnvironmentLightingSkyboxSource>(
          activeScene().environment));
  EXPECT_TRUE(res.saveScene);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SceneRemoveLightingActionTest,
       UndoAddsPreviousEnvironmentLightingForEnvironment) {
  activeScene().environment = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::EnvironmentLightingSkyboxSource>(
      activeScene().environment, {});

  liquid::editor::SceneRemoveLighting action;
  action.onExecute(state);

  auto res = action.onUndo(state);

  EXPECT_TRUE(
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
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SceneSetSkyboxLightingSourceActionTest,
       UndoRemovesSkyboxLightingSourceComponentFromEnvironment) {
  activeScene().environment = activeScene().entityDatabase.create();

  liquid::editor::SceneSetSkyboxLightingSource action;
  action.onExecute(state);

  auto res = action.onUndo(state);

  EXPECT_FALSE(
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
