#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquidator {

/**
 * @brief Scene view widget
 *
 * Renders the scene texture
 * in the view
 */
class SceneView {
public:
  /**
   * @brief Begin scene view
   *
   * @param texture Texture to show
   * @retval true Scene view is visible
   * @retval false Scene view is not visible
   */
  static bool begin(liquid::rhi::TextureHandle texture);

  /**
   * @brief End scene view
   */
  static void end();
};

} // namespace liquidator
