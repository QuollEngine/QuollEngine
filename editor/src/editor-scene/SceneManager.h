#pragma once

#include "scene/Scene.h"
#include "EditorCamera.h"

namespace liquidator {

class SceneManager {
public:
  /**
   * @brief Create scene manager
   *
   * @param context Entity context
   * @param editorCamera Editor camera
   */
  SceneManager(liquid::EntityContext &context, EditorCamera &editorCamera);

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
   * @brief Get active scene
   *
   * @return Active scene
   */
  inline liquid::Scene *getActiveScene() { return activeScene; }

  /**
   * @brief Check if new scene has been requested
   *
   * @retval true New scene is requested
   * @retval false New scene is not requested
   */
  inline bool hasNewScene() const { return newSceneRequested; }

  /**
   * @brief Creates a new scene and sets it as active
   */
  void createNewScene();

private:
  liquid::Scene *activeScene = nullptr;
  bool newSceneRequested = true;
  liquid::EntityContext &context;
  EditorCamera &editorCamera;
};

} // namespace liquidator
