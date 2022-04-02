#pragma once

#include "platform-tools/NativeFileDialog.h"
#include "liquid/loaders/GLTFLoader.h"
#include "../asset/GLTFImporter.h"
#include "liquid/scene/Scene.h"

#include "../editor-scene/SceneManager.h"

namespace liquidator {

class MenuBar {
public:
  /**
   * @brief Create menu bar
   *
   * @param loader GLTF mLoader
   * @param importer GLTF Importer (new)
   */
  MenuBar(const liquid::GLTFLoader &loader, GLTFImporter &importer);

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
   * @brief Handler for new GLTF import item click
   *
   * @param filePath GLTF file path
   */
  void handleAssetImport(const liquid::String &filePath);

  /**
   * @brief Handle new scene item click
   *
   * @param sceneManager Scene manager
   */
  void handleNewScene(SceneManager &sceneManager);

private:
  liquid::platform_tools::NativeFileDialog mFileDialog;
  liquid::GLTFLoader mLoader;
  GLTFImporter &mImporter;
};

} // namespace liquidator
