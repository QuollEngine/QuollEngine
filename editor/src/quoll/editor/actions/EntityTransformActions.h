#pragma once

#include "Action.h"

#include "quoll/scene/LocalTransform.h"

namespace quoll::editor {

class EntitySetLocalTransformContinuous : public Action {
public:
  EntitySetLocalTransformContinuous(
      Entity entity, std::optional<LocalTransform> oldLocalTransform,
      std::optional<LocalTransform> newLocalTransform = std::nullopt);

  void setNewComponent(LocalTransform newLocalTransform);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;

  std::optional<LocalTransform> mOldLocalTransform;
  std::optional<LocalTransform> mNewLocalTransform;
};

} // namespace quoll::editor
