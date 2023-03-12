#pragma once

#include "Action.h"

namespace liquid::editor {

/**
 * @brief Entity set local transform action
 */
class EntitySetText : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param text Text
   */
  EntitySetText(Entity entity, Text text);

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
  Text mText;
};

} // namespace liquid::editor
