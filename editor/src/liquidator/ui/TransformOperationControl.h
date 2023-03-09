#pragma once

#include "liquidator/core/TransformOperation.h"
#include "liquidator/state/WorkspaceState.h"

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
   */
  TransformOperationControl(WorkspaceState &state);

private:
  void renderIcon(TransformOperation transformOperation, WorkspaceState &state);

  static const char *
  getTransformOperationIcon(TransformOperation transformOperation);
};

} // namespace liquid::editor
