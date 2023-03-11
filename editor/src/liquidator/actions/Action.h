#pragma once

#include "liquidator/state/WorkspaceState.h"
#include "liquidator/ui/FontAwesome.h"

namespace liquid::editor {

/**
 * @brief Action executor result
 */
struct ActionExecutorResult {};

/**
 * @brief Editor action
 */
class Action {
public:
  /**
   * Action name
   */
  StringView name;

  /**
   * Action executor
   */
  std::function<ActionExecutorResult(WorkspaceState &, std::any data)>
      onExecute;

  /**
   * Action predicate
   */
  std::function<bool(WorkspaceState &)> predicate = [](WorkspaceState &) {
    return true;
  };
};

} // namespace liquid::editor
