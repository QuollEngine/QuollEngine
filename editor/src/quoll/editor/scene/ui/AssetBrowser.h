#pragma once

#include "quoll/asset/AssetCache.h"

#include "quoll/editor/asset/AssetLoader.h"
#include "quoll/editor/actions/ActionExecutor.h"

#include "quoll/editor/ui/AssetLoadStatusDialog.h"
#include "quoll/editor/ui/IconRegistry.h"
#include "quoll/editor/ui/MaterialViewer.h"

namespace quoll::editor {

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
  void render(WorkspaceState &state, AssetManager &assetManager,
              ActionExecutor &actionExecutor);

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
