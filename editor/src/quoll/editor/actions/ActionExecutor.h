#pragma once

#include "Action.h"
#include "quoll/editor/asset/AssetSyncer.h"

namespace quoll::editor {

/**
 * @brief Action executor
 */
class ActionExecutor {
  using ActionStack = std::list<std::unique_ptr<Action>>;

public:
  /**
   * @brief Create action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   */
  ActionExecutor(WorkspaceState &state, AssetRegistry &assetRegistry);

  /**
   * @brief Set asset syncer
   *
   * @param assetSyncer Asset syncer
   */
  void setAssetSyncer(AssetSyncer *assetSyncer);

  /**
   * @brief Process actions
   */
  void process();

  /**
   * @brief Execute action
   *
   * @param action Action
   */
  void execute(std::unique_ptr<Action> action);

  /**
   * @brief Execute action
   *
   * @tparam TAction Action type
   * @tparam ...TArgs Action type arguments
   * @param ...args Action arguments
   */
  template <typename TAction, typename... TArgs> void execute(TArgs &&...args) {
    execute(std::make_unique<TAction>(std::forward<TArgs>(args)...));
  }

  /**
   * @brief Undo last action
   */
  void undo();

  /**
   * @brief Redo last action
   */
  void redo();

  /**
   * @brief Get undo stack
   *
   * @return Undo stack
   */
  inline const ActionStack &getUndoStack() const { return mUndoStack; }

  /**
   * @brief Get redo stack
   *
   * @return Redo stack
   */
  inline const ActionStack &getRedoStack() const { return mRedoStack; }

private:
  /**
   * @brief Save action result
   *
   * @param result Action executor result
   */
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
