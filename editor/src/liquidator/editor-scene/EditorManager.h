#pragma once

#include "liquidator/project/Project.h"
#include "liquidator/state/WorkspaceState.h"

#include "EditorCamera.h"
#include "EntityManager.h"

namespace liquid::editor {

enum class EnvironmentLightingSource { None = 0, Skybox = 1 };

/**
 * @brief Editor manager
 *
 * Manages, saves, and loads the
 * scene with editor settings
 */
class EditorManager {
public:
  /**
   * @brief Create editor manager
   *
   * @param editorCamera Editor camera
   * @param entityManager Entity manager
   * @param project Project
   */
  EditorManager(EditorCamera &editorCamera, EntityManager &entityManager,
                const Project &project);

  EditorManager(const EditorManager &) = delete;
  EditorManager(EditorManager &&) = delete;
  EditorManager &operator=(const EditorManager &) = delete;
  EditorManager &operator=(EditorManager &&) = delete;
  ~EditorManager() = default;

  /**
   * @brief Save workspace state to a file
   *
   * @param state Workspace state
   * @param path Path to workspace state file
   */
  void saveWorkspaceState(WorkspaceState &state,
                          const std::filesystem::path &path);

  /**
   * @brief Load workspace state from file
   *
   * @param path Path to workspace state file
   * @param state Workspace state
   */
  void loadWorkspaceState(const std::filesystem::path &path,
                          WorkspaceState &state);

  /**
   * @brief Get editor camera
   *
   * @return Editor camera
   */
  inline EditorCamera &getEditorCamera() { return mEditorCamera; }

  /**
   * @brief Creates a new scene and sets it as active
   */
  void createNewScene();

  /**
   * @brief Load or create scene
   */
  void loadOrCreateScene();

  /**
   * @brief Check if skybox exists
   *
   * @retval true Environment skybox exists
   * @retval false Environment skybox does not exist
   */
  bool hasSkybox();

  /**
   * @brief Get skybox type
   *
   * @return Skybox type
   */
  EnvironmentSkyboxType getSkyboxType();

  /**
   * @brief Get skybox color
   *
   * @return Skybox color
   */
  const glm::vec4 &getSkyboxColor();

  /**
   * @brief Set skybox color
   *
   * @param color Skybox color
   */
  void setSkyboxColor(const glm::vec4 &color);

  /**
   * @brief Get skybox texture
   *
   * @return Environment asset handle
   */
  EnvironmentAssetHandle getSkyboxTexture();

  /**
   * @brief Set skybox texture
   *
   * @param environment Environment asset handle
   */
  void setSkyboxTexture(EnvironmentAssetHandle environment);

  /**
   * @brief Delete skybox
   */
  void removeSkybox();

  /**
   * @brief Get environment lighting source
   *
   * @return Environment lighting source
   */
  EnvironmentLightingSource getEnvironmentLightingSource();

  /**
   * @brief Remove environment lighting source
   *
   * @param save Save changes to scene file
   */
  void removeEnvironmentLightingSource(bool save);

  /**
   * @brief Set skybox as environment lighting source
   */
  void setEnvironmentLightingSkyboxSource();

private:
  EditorCamera &mEditorCamera;
  std::filesystem::path mScenePath;
  EntityManager &mEntityManager;
  Project mProject;
};

} // namespace liquid::editor
