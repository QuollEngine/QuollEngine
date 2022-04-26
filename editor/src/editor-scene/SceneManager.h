#pragma once

#include "liquid/scene/Scene.h"
#include "EditorCamera.h"
#include "EditorGrid.h"

namespace liquidator {

class SceneManager {
public:
  /**
   * @brief Create scene manager
   *
   * @param entityContext Entity context
   * @param editorCamera Editor camera
   * @param editorGrid Editor grid
   */
  SceneManager(liquid::EntityContext &entityContext, EditorCamera &editorCamera,
               EditorGrid &editorGrid);

  SceneManager(const SceneManager &) = delete;
  SceneManager(SceneManager &&) = delete;
  SceneManager &operator=(const SceneManager &) = delete;
  SceneManager &operator=(SceneManager &&) = delete;
  ~SceneManager() = default;

  /**
   * @brief Request a new empty scene
   */
  void requestEmptyScene();

  /**
   * @brief Create empty entity in camera view
   *
   * @return Scene node
   */
  liquid::SceneNode *createEntityAtView();

  /**
   * @brief Get active scene
   *
   * @return Active scene
   */
  inline liquid::Scene *getActiveScene() { return mActiveScene; }

  /**
   * @brief Check if new scene has been requested
   *
   * @retval true New scene is requested
   * @retval false New scene is not requested
   */
  inline bool hasNewScene() const { return mNewSceneRequested; }

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

private:
  liquid::Scene *mActiveScene = nullptr;
  bool mNewSceneRequested = true;
  liquid::EntityContext &mEntityContext;
  EditorCamera &mEditorCamera;
  EditorGrid &mEditorGrid;
};

} // namespace liquidator
