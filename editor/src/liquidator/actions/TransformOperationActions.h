#pragma once

#include "liquidator/actions/Action.h"

namespace quoll::editor {

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
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  TransformOperation mTransformOperation;
  TransformOperation mOldTransformOperation{};
};

} // namespace quoll::editor
