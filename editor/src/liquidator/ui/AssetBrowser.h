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
    Path path;
    String name;
    String truncatedName;
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
  void handleAssetImport(AssetManager &assetManager);

  void handleCreateEntry(AssetManager &assetManager);

  void renderEntry(const Entry &entry);

  void fetchAssetDirectory(Path path, AssetManager &assetManager);

  void fetchPrefab(PrefabAssetHandle handle, AssetManager &assetManager);

  void setDefaultProps(Entry &entry, AssetRegistry &assetRegistry);

  void setCurrentFetch(std::variant<Path, PrefabAssetHandle> fetch);

  const Path &getCurrentFetchPath() const;

private:
  Entry mStagingEntry;
  bool mHasStagingEntry = false;
  bool mInitialFocusSet = false;

  bool mNeedsRefresh = true;
  std::variant<Path, PrefabAssetHandle> mCurrentFetch;

  std::vector<Entry> mEntries;
  Path mCurrentDirectory;
  Path mPrefabDirectory;
  size_t mSelected = std::numeric_limits<size_t>::max();

  AssetLoadStatusDialog mStatusDialog{"AssetLoadStatus"};
  MaterialViewer mMaterialViewer;
};

} // namespace liquid::editor
