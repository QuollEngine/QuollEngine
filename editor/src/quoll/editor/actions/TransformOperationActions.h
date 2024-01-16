#pragma once

#include "Action.h"

namespace quoll::editor {

class SetActiveTransform : public Action {
public:
  SetActiveTransform(TransformOperation transformOperation);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  TransformOperation mTransformOperation;
  TransformOperation mOldTransformOperation{};
};

} // namespace quoll::editor
