#pragma once

#include "quoll/scene/LocalTransform.h"
#include "Action.h"

namespace quoll::editor {

class EntitySetLocalTransformContinuous : public Action {
public:
  EntitySetLocalTransformContinuous(
      Entity entity, std::optional<LocalTransform> oldLocalTransform,
      std::optional<LocalTransform> newLocalTransform = std::nullopt);

  void setNewComponent(LocalTransform newLocalTransform);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  Entity mEntity;

  std::optional<LocalTransform> mOldLocalTransform;
  std::optional<LocalTransform> mNewLocalTransform;
};

} // namespace quoll::editor
