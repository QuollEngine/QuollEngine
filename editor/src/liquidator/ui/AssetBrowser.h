#pragma once

#include "liquid/asset/AssetCache.h"

#include "liquidator/asset/AssetLoader.h"
#include "liquidator/editor-scene/EditorManager.h"
#include "liquidator/actions/ActionExecutor.h"

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
    rhi::TextureHandle preview = rhi::TextureHandle::Null;
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
   * @param state Workspace state
   * @param actionExecutor Action executor
   */
  void render(AssetManager &assetManager, IconRegistry &iconRegistry,
              WorkspaceState &state, ActionExecutor &actionExecutor);

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
  Path mAssetDirectory;
  Path mContentsDirectory;
  bool mDirectoryChanged = true;
  size_t mSelected = std::numeric_limits<size_t>::max();
  AssetLoader &mAssetLoader;

  AssetLoadStatusDialog mStatusDialog;
  MaterialViewer mMaterialViewer;
};

} // namespace liquid::editor
