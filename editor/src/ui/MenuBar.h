#pragma once

#include "platform-tools/NativeFileDialog.h"
#include "loaders/TinyGLTFLoader.h"
#include "scene/Scene.h"

namespace liquidator {

class MenuBar {
public:
  /**
   * @brief Create menu bar
   *
   * @param loader GLTF loader
   */
  MenuBar(const liquid::TinyGLTFLoader &loader);

  /**
   * @brief Render menu bar
   *
   * @param scene Scene
   */
  void render(liquid::Scene *scene);

private:
  /**
   * @brief Handler for GLTF import item click
   *
   * @param filePath GLTF file path
   * @param scene Target scene
   */
  void handleGLTFImport(const liquid::String &filePath, liquid::Scene *scene);

private:
  liquid::platform_tools::NativeFileDialog fileDialog;
  liquid::TinyGLTFLoader loader;
};

} // namespace liquidator
