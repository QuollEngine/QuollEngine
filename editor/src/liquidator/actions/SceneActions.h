#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Set starting camera for scene action
 */
class SceneSetStartingCamera : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  SceneSetStartingCamera(Entity entity);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  Entity mEntity;
};

/**
 * @brief Remove skybox from scene action
 */
class SceneRemoveSkybox : public Action {
public:
  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;
};

/**
 * @brief Set skybox color action
 */
class SceneSetSkyboxColor : public Action {
public:
  /**
   * @brief Create action
   *
   * @param color Skybox color
   */
  SceneSetSkyboxColor(glm::vec4 color);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  glm::vec4 mColor;
};

/**
 * @brief Set skybox texture action
 */
class SceneSetSkyboxTexture : public Action {
public:
  /**
   * @brief Create action
   *
   * @param texture Skybox texture
   */
  SceneSetSkyboxTexture(liquid::EnvironmentAssetHandle texture);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  liquid::EnvironmentAssetHandle mTexture;
};

/**
 * @brief Remove environment lighting from scene action
 */
class SceneRemoveLighting : public Action {
public:
  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;
};

/**
 * @brief Set skybox environment lighting source for scene actio
 */
class SceneSetSkyboxLightingSource : public Action {
public:
  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;
};

} // namespace liquid::editor
