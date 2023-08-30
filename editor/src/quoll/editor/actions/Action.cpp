#include "quoll/core/Base.h"
#include "Action.h"

namespace quoll::editor {

ActionExecutorResult Action::onUndo(WorkspaceState &state,
                                    AssetRegistry &assetRegistry) {
  return {};
}

} // namespace quoll::editor
