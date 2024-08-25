#pragma once

#include "Action.h"

namespace quoll::editor {

class EntitySetAudio : public Action {
public:
  EntitySetAudio(Entity entity, AssetHandle<AudioAsset> audio);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AssetHandle<AudioAsset> mAudio;
  AssetHandle<AudioAsset> mOldAudio;
};

class EntityCreateAudio : public Action {
public:
  EntityCreateAudio(Entity entity, AssetHandle<AudioAsset> handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AssetHandle<AudioAsset> mHandle;
};

} // namespace quoll::editor
