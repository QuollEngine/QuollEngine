#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Action executor
 */
class ActionExecutor {
public:
  /**
   * @brief Create action executor
   *
   * @param state Workspace state
   */
  ActionExecutor(WorkspaceState &state);

  /**
   * @brief Execute action
   *
   * @param action Action
   * @param data Action data
   */
  void execute(const Action &action, std::any data = {});

private:
  WorkspaceState &mState;
};

} // namespace liquid::editor
