#pragma once

#include "platform-tools/NativeFileDialog.h"
#include "liquid/loaders/GLTFLoader.h"
#include "liquid/scene/Scene.h"

#include "../editor-scene/SceneManager.h"

namespace liquidator {

class MenuBar {
public:
  /**
   * @brief Create menu bar
   *
   * @param loader GLTF loader
   */
  MenuBar(const liquid::GLTFLoader &loader);

  /**
   * @brief Render menu bar
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

private:
  /**
   * @brief Handler for GLTF import item click
   *
   * @param filePath GLTF file path
   * @param scene Target scene
   */
  void handleGLTFImport(const liquid::String &filePath, liquid::Scene *scene);

  /**
   * @brief Handle new scene item click
   *
   * @param sceneManager Scene manager
   */
  void handleNewScene(SceneManager &sceneManager);

private:
  liquid::platform_tools::NativeFileDialog fileDialog;
  liquid::GLTFLoader loader;
};

} // namespace liquidator
