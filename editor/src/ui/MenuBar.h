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
   * @param importer GLTF Importer (new)
   */
  MenuBar(GLTFImporter &importer);

  /**
   * @brief Render menu bar
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

private:
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
  GLTFImporter &mImporter;
};

} // namespace liquidator
