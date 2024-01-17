#include "quoll/core/Base.h"
#include "VulkanHeaders.h"

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
// NOLINTEND(cppcoreguidelines-macro-usage)

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
