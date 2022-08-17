#pragma once

#include "IconRegistry.h"
#include "liquidator/core/TransformOperation.h"
#include "liquidator/editor-scene/EditorManager.h"

namespace liquidator {

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
   * @param iconRegistry Icon registry
   * @param editorManager Editor manager
   * @param iconSize Icon size
   */
  TransformOperationControl(const IconRegistry &iconRegistry,
                            EditorManager &editorManager, float iconSize);

  /**
   * @brief Default destructor
   */
  ~TransformOperationControl() = default;

  TransformOperationControl(const TransformOperationControl &) = delete;
  TransformOperationControl(TransformOperationControl &&) = delete;
  TransformOperationControl &
  operator=(const TransformOperationControl &) = delete;
  TransformOperationControl &operator=(TransformOperationControl &&) = delete;

private:
  void renderIcon(const IconRegistry &iconRegistry,
                  TransformOperation transformOperation,
                  EditorManager &editorManager, float iconSize);

  static EditorIcon
  getTransformOperationIcon(TransformOperation transformOperation);
};

} // namespace liquidator
