#pragma once

#include "Action.h"

namespace quoll::editor {

class EntitySetAudio : public Action {
public:
  EntitySetAudio(Entity entity, AssetHandle<AudioAsset> audio);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  AssetHandle<AudioAsset> mAudio;
  AssetHandle<AudioAsset> mOldAudio;
};

class EntityCreateAudio : public Action {
public:
  EntityCreateAudio(Entity entity, AssetHandle<AudioAsset> handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;
  AssetHandle<AudioAsset> mHandle;
};

} // namespace quoll::editor
