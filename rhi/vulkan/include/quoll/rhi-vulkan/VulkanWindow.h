#pragma once

#include "quoll/window/Window.h"
#include "VulkanHeaders.h"

namespace quoll::rhi {

VkSurfaceKHR createSurfaceFromWindow(VkInstance instance, Window &window);

} // namespace quoll::rhi
