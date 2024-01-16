#pragma once

#include "VulkanHeaders.h"
#include "quoll/window/Window.h"

namespace quoll::rhi {

VkSurfaceKHR createSurfaceFromWindow(VkInstance instance, Window &window);

} // namespace quoll::rhi
