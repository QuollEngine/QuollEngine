#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Set script for entity action
 */
class EntitySetScript : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param script Script handle
   */
  EntitySetScript(Entity entity, LuaScriptAssetHandle script);

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
  LuaScriptAssetHandle mScript;
};

} // namespace liquid::editor
