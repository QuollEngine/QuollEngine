#pragma once

#include "Action.h"

namespace liquid::editor {

/**
 * @brief Entity set parent action
 */
class EntitySetParent : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param parent Parent
   */
  EntitySetParent(Entity entity, Entity parent);

  /**
   * @brief Executor
   *
   * @param state Workspace state
   * @return Execution result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Undo
   *
   * @param state Workspace state
   * @return Execution result
   */
  ActionExecutorResult onUndo(WorkspaceState &state) override;

  /**
   * @brief Predicate
   *
   * @param state Workspace state
   * @retval true Parent is valid
   * @retval false Parent is not valid
   */
  bool predicate(WorkspaceState &state) override;

private:
  Entity mEntity;
  Entity mParent;
  Entity mPreviousParent = Entity::Null;
};

} // namespace liquid::editor
