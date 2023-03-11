#pragma once

#include "liquidator/core/TransformOperation.h"
#include "liquidator/state/WorkspaceState.h"
#include "liquidator/actions/ActionExecutor.h"

namespace liquid::editor {

/**
 * @brief Transform operation control widget
 *
 * Renders the transform operations and
 * selects the active one
 */
class TransformOperationControl {
public:
  /**
   * @brief Begin toolbar
   *
   * @param state Workspace state
   * @param actionExecutor Action executor
   */
  TransformOperationControl(WorkspaceState &state,
                            ActionExecutor &actionExecutor);

private:
  static constexpr size_t NumOperations = 3;
  std::array<Action, NumOperations> mActions;
};

} // namespace liquid::editor
