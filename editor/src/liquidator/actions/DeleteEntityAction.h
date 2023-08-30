#pragma once

#include "liquidator/actions/Action.h"

namespace quoll::editor {

/**
 * @brief Delete entity action
 */
class DeleteEntity : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity to delete
   */
  DeleteEntity(Entity entity);

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
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
};

} // namespace quoll::editor
