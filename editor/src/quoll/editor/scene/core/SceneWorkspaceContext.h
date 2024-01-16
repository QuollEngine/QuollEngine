#pragma once

#include "quoll/editor/workspace/WorkspaceState.h"
#include "quoll/editor/actions/ActionExecutor.h"
#include "quoll/editor/asset/AssetManager.h"

namespace quoll::editor {

struct WorkspaceContext {
  WorkspaceState &state;

  ActionExecutor &actionExecutor;

  AssetManager &assetManager;
};

} // namespace quoll::editor
