#include "liquid/core/Base.h"
#include "Action.h"

namespace liquid::editor {

ActionExecutorResult Action::onUndo(WorkspaceState &state) { return {}; }

} // namespace liquid::editor
