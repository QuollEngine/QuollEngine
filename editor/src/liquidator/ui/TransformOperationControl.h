#pragma once

#include "liquidator/core/TransformOperation.h"
#include "liquidator/editor-scene/EditorManager.h"

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
   * @param editorManager Editor manager
   */
  TransformOperationControl(EditorManager &editorManager);

private:
  void renderIcon(TransformOperation transformOperation,
                  EditorManager &editorManager);

  static const char *
  getTransformOperationIcon(TransformOperation transformOperation);
};

} // namespace liquid::editor
