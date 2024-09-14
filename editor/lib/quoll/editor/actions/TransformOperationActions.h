#pragma once

#include "Action.h"

namespace quoll::editor {

class SetActiveTransform : public Action {
public:
  SetActiveTransform(TransformOperation transformOperation);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

private:
  TransformOperation mTransformOperation;
  TransformOperation mOldTransformOperation{};
};

} // namespace quoll::editor
