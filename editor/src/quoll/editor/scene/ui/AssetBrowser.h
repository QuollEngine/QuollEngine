#pragma once

#include "quoll/editor/ui/AssetLoadStatusDialog.h"
#include "quoll/editor/ui/MaterialViewer.h"

namespace quoll::editor {

struct WorkspaceState;
class AssetManager;
class ActionExecutor;

class AssetBrowser {
  enum class PathType { Directory, ComplexAsset, SimpleAsset };

  struct Entry {
    PathType pathType;
    Path path;
    Uuid uuid;
    String name;
    String truncatedName;
    f32 textWidth = 0.0f;
    rhi::TextureHandle preview = rhi::TextureHandle::Null;
    AssetType assetType = AssetType::None;
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

  void fetchAssetContents(Path path, AssetManager &assetManager);

  void setDefaultProps(Entry &entry, AssetManager &assetManager);

  void setCurrentFetch(Path path, PathType pathType);

private:
  Entry mStagingEntry;
  bool mHasStagingEntry = false;
  bool mInitialFocusSet = false;

  bool mNeedsRefresh = true;

  std::vector<Entry> mEntries;
  Path mCurrentPath;
  PathType mCurrentPathType;

  usize mSelected = std::numeric_limits<usize>::max();

  AssetLoadStatusDialog mStatusDialog{"AssetLoadStatus"};
  MaterialViewer mMaterialViewer;
};

} // namespace quoll::editor
