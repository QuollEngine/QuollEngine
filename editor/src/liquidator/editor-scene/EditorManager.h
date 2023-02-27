#pragma once

#include "liquidator/project/Project.h"
#include "liquidator/core/TransformOperation.h"

#include "EditorCamera.h"
#include "EditorGrid.h"
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
   * @param editorGrid Editor grid
   * @param entityManager Entity manager
   * @param project Project
   */
  EditorManager(EditorCamera &editorCamera, EditorGrid &editorGrid,
                EntityManager &entityManager, const Project &project);

  EditorManager(const EditorManager &) = delete;
  EditorManager(EditorManager &&) = delete;
  EditorManager &operator=(const EditorManager &) = delete;
  EditorManager &operator=(EditorManager &&) = delete;
  ~EditorManager() = default;

  /**
   * @brief Save editor state to a file
   *
   * @param path Path to editor state file
   */
  void saveEditorState(const std::filesystem::path &path);

  /**
   * @brief Load editor state from file
   *
   * @param path Path to editor state file
   */
  void loadEditorState(const std::filesystem::path &path);

  /**
   * @brief Get editor camera
   *
   * @return Editor camera
   */
  inline EditorCamera &getEditorCamera() { return mEditorCamera; }

  /**
   * @brief Get editor grid
   *
   * @return Editor grid
   */
  inline EditorGrid &getEditorGrid() { return mEditorGrid; }

  /**
   * @brief Creates a new scene and sets it as active
   */
  void createNewScene();

  /**
   * @brief Load or create scene
   */
  void loadOrCreateScene();

  /**
   * @brief Move camera to entity location
   *
   * @param entity Entity
   */
  void moveCameraToEntity(Entity entity);

  /**
   * @brief Check if environment skybox exists
   *
   * @retval true Environment skybox exists
   * @retval false Environment skybox does not exist
   */
  bool hasEnvironmentSkybox();

  /**
   * @brief Get environment skybox
   *
   * @return Environment asset handle
   */
  EnvironmentAssetHandle getEnvironmentSkybox();

  /**
   * @brief Set environment skybox
   *
   * @param environment Environment asset handle
   */
  void setEnvironmentSkybox(EnvironmentAssetHandle environment);

  /**
   * @brief Delete environment skybox
   */
  void deleteEnvironmentSkybox();

  /**
   * @brief Set transform operation
   *
   * @param transformOperation Transform operation
   */
  void setTransformOperation(TransformOperation transformOperation);

  /**
   * @brief Get environment lighting source
   *
   * @return Environment lighting source
   */
  EnvironmentLightingSource getEnvironmentLightingSource();

  /**
   * @brief Remove environment lighting source
   */
  void removeEnvironmentLightingSource();

  /**
   * @brief Set skybox as environment lighting source
   */
  void setEnvironmentLightingSkyboxSource();

  /**
   * @brief Get transform operation
   *
   * @return Transform operation
   */
  TransformOperation getTransformOperation() const {
    return mTransformOperation;
  }

  /**
   * @brief Start game export flow
   */
  void startGameExport();

private:
  EditorCamera &mEditorCamera;
  EditorGrid &mEditorGrid;
  std::filesystem::path mScenePath;
  EntityManager &mEntityManager;
  uint32_t mLastId = 1;
  Project mProject;

  TransformOperation mTransformOperation = TransformOperation::Move;

  Entity mEnvironmentEntity = EntityNull;
};

} // namespace liquid::editor
