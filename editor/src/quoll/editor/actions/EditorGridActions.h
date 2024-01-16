#pragma once

#include "Action.h"

namespace quoll::editor {

class SetGridLines : public Action {
public:
  static bool isShown(WorkspaceState &state);

public:
  SetGridLines(bool show);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  bool mShow = false;
};

class SetGridAxisLines : public Action {
public:
  static bool isShown(WorkspaceState &state);

public:
  SetGridAxisLines(bool show);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  bool mShow = false;
};

} // namespace quoll::editor
