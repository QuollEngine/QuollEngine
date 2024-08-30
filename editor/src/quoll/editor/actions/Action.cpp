#include "quoll/core/Base.h"
#include "Action.h"

namespace quoll::editor {

ActionExecutorResult Action::onUndo(WorkspaceState &state,
                                    AssetCache &assetCache) {
  return {};
}

} // namespace quoll::editor
