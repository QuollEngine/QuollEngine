#pragma once

#include "liquid/asset/AssetManager.h"
#include "liquid/platform-tools/NativeFileDialog.h"
#include "liquid/platform-tools/NativeFileOpener.h"

#include "liquidator/asset/AssetLoader.h"
#include "liquidator/editor-scene/EditorManager.h"

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
  void render(liquid::AssetManager &assetManager, IconRegistry &iconRegistry,
              EditorManager &editorManager, EntityManager &entityManager);

  /**
   * @brief Reload contents in current directory
   */
  void reload();

  /**
   * @brief Set create entry handler
   *
   * @param handler Create entry handler
   */
  void setOnCreateEntry(std::function<void(liquid::Path)> handler);

private:
  /**
   * @brief Handle importing assets
   */
  void handleAssetImport();

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
  AssetLoader &mAssetLoader;
  liquid::platform_tools::NativeFileDialog mFileDialog;
  liquid::platform_tools::NativeFileOpener mFileOpener;

  std::function<void(liquid::Path)> mOnCreateEntry;

  AssetLoadStatusDialog mStatusDialog;
};

} // namespace liquidator
