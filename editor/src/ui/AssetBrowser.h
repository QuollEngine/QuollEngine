#pragma once

#include "liquid/asset/AssetManager.h"
#include "platform-tools/NativeFileDialog.h"

#include "../asset/GLTFImporter.h"
#include "../editor-scene/EditorManager.h"

#include "IconRegistry.h"
#include "AssetLoadStatusDialog.h"

namespace liquidator {

/**
 * @brief Asset browser component
 */
class AssetBrowser {
  struct Entry {
    std::filesystem::path path;
    liquid::String clippedName;
    float textWidth = 0.0f;
    bool isDirectory = false;
    EditorIcon icon = EditorIcon::Unknown;
    liquid::rhi::TextureHandle preview = liquid::rhi::TextureHandle::Invalid;
    liquid::AssetType assetType = liquid::AssetType::None;
    uint32_t asset = 0;
    bool isEditable = false;
  };

public:
  /**
   * @brief Create asset browser
   *
   * @param gltfImporter GLTF importer
   */
  AssetBrowser(GLTFImporter &gltfImporter);

  /**
   * @brief Render status bar
   *
   * @param assetManager Asset manager
   * @param iconRegistry Icon registry
   * @param editorManager Editor manager
   * @param entityManager Entity manager
   */
  void render(liquid::AssetManager &assetManager, IconRegistry &iconRegistry,
              EditorManager &editorManager, EntityManager &entityManager);

  /**
   * @brief Reload contents in current directory
   */
  void reload();

private:
  /**
   * @brief Handle GLTF import
   */
  void handleGLTFImport();

  /**
   * @brief Handle entry creation
   */
  void handleCreateEntry();

  /**
   * @brief Render entry
   *
   * @param entry Entry
   */
  void renderEntry(const Entry &entry);

private:
  Entry mStagingEntry;
  bool mHasStagingEntry = false;
  bool mInitialFocusSet = false;

  std::vector<Entry> mEntries;
  std::filesystem::path mCurrentDirectory;
  bool mDirectoryChanged = true;
  size_t mSelected = std::numeric_limits<size_t>::max();
  GLTFImporter &mGltfImporter;
  liquid::platform_tools::NativeFileDialog mFileDialog;

  AssetLoadStatusDialog mStatusDialog;
};

} // namespace liquidator
