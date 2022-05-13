#pragma once

#include "EditorCamera.h"
#include "EditorGrid.h"

#include "EntityManager.h"

namespace liquidator {

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
   */
  EditorManager(EditorCamera &editorCamera, EditorGrid &editorGrid,
                EntityManager &entityManager);

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
   * @brief Get camera
   *
   * @return Camera entity
   */
  inline liquid::Entity getCamera() { return mCameraEntity; }

  /**
   * @brief Set camera
   *
   * @param camera Camera entity
   */
  void setCamera(liquid::Entity camera);

  /**
   * @brief Switch to editor camera
   */
  void switchToEditorCamera();

  /**
   * @brief Check if editor camera is active
   *
   * @retval true Editor camera is active
   * @retval false Editor camera is not active
   */
  inline bool isUsingEditorCamera() const {
    return mCameraEntity == mEditorCamera.getCamera();
  }

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
  void moveCameraToEntity(liquid::Entity entity);

  /**
   * @brief Check if environment exists
   *
   * @retval true Environment exists
   * @retval false Environment does not exist
   */
  bool hasEnvironment();

  /**
   * @brief Get environment
   *
   * @return Environment component
   */
  liquid::EnvironmentComponent &getEnvironment();

private:
  EditorCamera &mEditorCamera;
  EditorGrid &mEditorGrid;
  std::filesystem::path mScenePath;
  EntityManager &mEntityManager;
  uint32_t mLastId = 1;

  liquid::Entity mEnvironmentEntity = liquid::ENTITY_MAX;
  liquid::Entity mCameraEntity = liquid::ENTITY_MAX;
};

} // namespace liquidator
