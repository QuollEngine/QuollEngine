#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Set name for entity action
 */
class EntitySetName : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param name Name
   */
  EntitySetName(Entity entity, Name name);

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
  Entity mEntity;
  Name mName;
};

} // namespace liquid::editor
