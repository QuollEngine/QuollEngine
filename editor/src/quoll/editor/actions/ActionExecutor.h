#pragma once

#include "quoll/editor/asset/AssetSyncer.h"
#include "Action.h"

namespace quoll::editor {

class ActionExecutor {
  using ActionStack = std::list<std::unique_ptr<Action>>;

public:
  ActionExecutor(WorkspaceState &state, AssetRegistry &assetRegistry);

  void setAssetSyncer(AssetSyncer *assetSyncer);

  void process();

  void execute(std::unique_ptr<Action> action);

  template <typename TAction, typename... TArgs> void execute(TArgs &&...args) {
    execute(std::make_unique<TAction>(std::forward<TArgs>(args)...));
  }

  void undo();

  void redo();

  inline const ActionStack &getUndoStack() const { return mUndoStack; }

  inline const ActionStack &getRedoStack() const { return mRedoStack; }

private:
  void saveActionResult(const ActionExecutorResult &result);

private:
  WorkspaceState &mState;
  AssetRegistry &mAssetRegistry;
  Path mScenePath;
  AssetSyncer *mAssetSyncer = nullptr;

  std::unique_ptr<Action> mActionToProcess;
  ActionStack mUndoStack;
  ActionStack mRedoStack;
};

} // namespace quoll::editor
