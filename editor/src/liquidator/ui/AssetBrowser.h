#pragma once

#include "liquid/asset/AssetCache.h"
#include "liquid/platform-tools/NativeFileDialog.h"
#include "liquid/platform-tools/NativeFileOpener.h"

#include "liquidator/asset/AssetLoader.h"
#include "liquidator/editor-scene/EditorManager.h"

#include "IconRegistry.h"
#include "AssetLoadStatusDialog.h"

#include "MaterialViewer.h"

namespace liquid::editor {

/**
 * @brief Asset browser component
 */
class AssetBrowser {
  struct Entry {
    std::filesystem::path path;
    String clippedName;
    float textWidth = 0.0f;
    bool isDirectory = false;
    EditorIcon icon = EditorIcon::Unknown;
    rhi::TextureHandle preview = rhi::TextureHandle::Invalid;
    AssetType assetType = AssetType::None;
    uint32_t asset = 0;
    bool isEditable = false;
  };

public:
  /**
   * @brief Create asset browser
   *
   * @param assetLoader Asset loader
   */
  AssetBrowser(AssetLoader &assetLoader);

  /**
   * @brief Render status bar
   *
   * @param assetManager Asset manager
   * @param iconRegistry Icon registry
   * @param editorManager Editor manager
   * @param entityManager Entity manager
   */
  void render(AssetManager &assetManager, IconRegistry &iconRegistry,
              EditorManager &editorManager, EntityManager &entityManager);

  /**
   * @brief Reload contents in current directory
   */
  void reload();

private:
  /**
   * @brief Handle importing assets
   */
  void handleAssetImport();

  /**
   * @brief Handle entry creation
   *
   * @param assetCache Asset cache
   */
  void handleCreateEntry(AssetManager &assetManager);

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
  AssetLoader &mAssetLoader;
  platform_tools::NativeFileDialog mFileDialog;
  platform_tools::NativeFileOpener mFileOpener;

  AssetLoadStatusDialog mStatusDialog;
  MaterialViewer mMaterialViewer;
};

} // namespace liquid::editor
