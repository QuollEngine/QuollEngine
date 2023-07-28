#pragma once

#include "liquid/asset/AssetCache.h"

#include "liquidator/asset/AssetLoader.h"
#include "liquidator/actions/ActionExecutor.h"
#include "liquidator/workspace/WorkspaceContext.h"

#include "AssetLoadStatusDialog.h"

#include "IconRegistry.h"
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
   * @brief Render status bar
   *
   * @param context Workspace context
   */
  void render(WorkspaceContext &context);

  /**
   * @brief Reload contents in current directory
   */
  void reload();

private:
  /**
   * @brief Handle importing assets
   *
   * @param assetManager Asset manager
   */
  void handleAssetImport(AssetManager &assetManager);

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

  AssetLoadStatusDialog mStatusDialog{"AssetLoadStatus"};
  MaterialViewer mMaterialViewer;
};

} // namespace liquid::editor
