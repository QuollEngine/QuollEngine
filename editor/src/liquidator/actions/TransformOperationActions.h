#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Set active transform action
 */
class SetActiveTransform : public Action {
public:
  /**
   * @brief Create action
   *
   * @param transformOperation Transform operation
   */
  SetActiveTransform(TransformOperation transformOperation);

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
  TransformOperation mTransformOperation;
};

} // namespace liquid::editor
