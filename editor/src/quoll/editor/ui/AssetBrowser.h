#pragma once

#include "quoll/asset/AssetCache.h"

#include "quoll/editor/asset/AssetLoader.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/workspace/WorkspaceContext.h"

#include "AssetLoadStatusDialog.h"

#include "IconRegistry.h"
#include "MaterialViewer.h"

namespace quoll::editor {

/**
 * @brief Asset browser component
 */
class AssetBrowser {
  struct Entry {
    Path path;
    Uuid uuid;
    String name;
    String truncatedName;
    f32 textWidth = 0.0f;
    bool isDirectory = false;
    EditorIcon icon = EditorIcon::Unknown;
    rhi::TextureHandle preview = rhi::TextureHandle::Null;
    AssetType assetType = AssetType::None;
    u32 asset = 0;
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
  usize mSelected = std::numeric_limits<usize>::max();

  AssetLoadStatusDialog mStatusDialog{"AssetLoadStatus"};
  MaterialViewer mMaterialViewer;
};

} // namespace quoll::editor
