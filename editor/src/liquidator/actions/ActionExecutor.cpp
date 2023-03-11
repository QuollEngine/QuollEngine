#include "liquid/core/Base.h"
#include "ActionExecutor.h"

namespace liquid::editor {

ActionExecutor::ActionExecutor(WorkspaceState &state) : mState(state) {}

void ActionExecutor::execute(const Action &action, std::any data) {
  LIQUID_ASSERT((bool)action.onExecute,
                "Action \"" + String(action.name) + "\" has no executor");
  action.onExecute(mState, data);
}

} // namespace liquid::editor
