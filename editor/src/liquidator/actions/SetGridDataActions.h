#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Set grid lines action
 */
class SetGridLinesAction : public Action {
public:
  /**
   * @brief Check if grid lines are shown
   *
   * @param state Workspace state
   * @retval true Grid lines are shown
   * @retval false Grid lines are not shown
   */
  static bool isShown(WorkspaceState &state);

public:
  /**
   * @brief Create action
   *
   * @param show Show grid lines
   */
  SetGridLinesAction(bool show);

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
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  bool mShow = false;
};

/**
 * @brief Set grid axis lines action
 */
class SetGridAxisLinesAction : public Action {
public:
  /**
   * @brief Check if grid axis lines are shown
   *
   * @param state Workspace state
   * @retval true Grid axis lines are shown
   * @retval false Grid axis lines are not shown
   */
  static bool isShown(WorkspaceState &state);

public:
  /**
   * @brief Create action
   *
   * @param show Show grid axis lines
   */
  SetGridAxisLinesAction(bool show);

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
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  bool mShow = false;
};

} // namespace liquid::editor
