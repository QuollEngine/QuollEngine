#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace quoll::editor {

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
   * @param texture Scene texture
   */
  SceneView(rhi::TextureHandle texture);

  /**
   * @brief End scene view
   */
  ~SceneView();

  SceneView(const SceneView &) = delete;
  SceneView(SceneView &&) = delete;
  SceneView &operator=(const SceneView &) = delete;
  SceneView &operator=(SceneView &&) = delete;

  /**
   * @brief Check if scene view is expanded
   *
   * @retval true Scene view is expanded
   * @retval false Scene view is not expanded
   */
  inline operator bool() const { return mExpanded; }

private:
  bool mExpanded = false;
};

} // namespace quoll::editor
