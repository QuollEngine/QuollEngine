#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "UIComponents.h"

namespace quoll {

/**
 * @brief Canvas for rendering UI
 */
struct UICanvas {
  /**
   * Root view
   */
  UIView rootView;

  /**
   * Yoga layout tree
   */
  YGNodeRef flexRoot = nullptr;
};

} // namespace quoll
