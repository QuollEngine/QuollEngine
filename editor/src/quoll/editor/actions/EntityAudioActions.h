#pragma once

#include "Action.h"

namespace quoll::editor {

class EntitySetAudio : public Action {
public:
  EntitySetAudio(Entity entity, AudioAssetHandle audio);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AudioAssetHandle mAudio;
  AudioAssetHandle mOldAudio = AudioAssetHandle::Null;
};

class EntityCreateAudio : public Action {
public:
  EntityCreateAudio(Entity entity, AudioAssetHandle handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  AudioAssetHandle mHandle;
};

} // namespace quoll::editor
