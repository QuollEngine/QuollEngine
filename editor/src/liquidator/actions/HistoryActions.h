#pragma once

#include "liquidator/actions/ActionExecutor.h"

namespace liquid::editor {

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
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Action undo stack is not empty
   * @retval false Action undo stack is empty
   */
  bool predicate(WorkspaceState &state) override;

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
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Action redo stack is not empty
   * @retval false Action redo stack is empty
   */
  bool predicate(WorkspaceState &state) override;

private:
  ActionExecutor &mActionExecutor;
};

} // namespace liquid::editor
