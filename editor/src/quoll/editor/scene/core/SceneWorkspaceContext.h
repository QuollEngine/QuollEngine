#pragma once

namespace quoll::editor {

class AssetManager;
class ActionExecutor;
class WorkspaceState;

struct WorkspaceContext {
  WorkspaceState &state;

  ActionExecutor &actionExecutor;

  AssetManager &assetManager;
};

} // namespace quoll::editor
