#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "UIComponents.h"

namespace quoll {

struct UICanvas {
  UIView rootView;

  YGNodeRef flexRoot = nullptr;
};

} // namespace quoll
