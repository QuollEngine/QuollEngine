#pragma once

#include "quoll/editor/actions/ActionExecutor.h"

namespace quoll::editor {

/**
 * @brief Undo action
 */
class Undo : public Action {
public:
  /**
   * @brief Create action
   *
   * @param actionExecutor Action executor
   */
  Undo(ActionExecutor &actionExecutor);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Action undo stack is not empty
   * @retval false Action undo stack is empty
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  ActionExecutor &mActionExecutor;
};

/**
 * @brief Redo action
 */
class Redo : public Action {
public:
  /**
   * @brief Create action
   *
   * @param actionExecutor Action executor
   */
  Redo(ActionExecutor &actionExecutor);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Action redo stack is not empty
   * @retval false Action redo stack is empty
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  ActionExecutor &mActionExecutor;
};

} // namespace quoll::editor
