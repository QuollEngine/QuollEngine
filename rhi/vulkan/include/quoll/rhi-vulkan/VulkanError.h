#pragma once

#include "quoll/core/Engine.h"
#include "VulkanHeaders.h"

namespace quoll::rhi {

String createVulkanErrorMessage(VkResult resultCode, const String &errorMessage,
                                const String &debugName);

inline void checkForVulkanError(VkResult resultCode, const String &errorMessage,
                                const String &debugName = "") {
  QuollAssert(resultCode == VK_SUCCESS,
              createVulkanErrorMessage(resultCode, errorMessage, debugName));
  if (resultCode != VK_SUCCESS) {
    Engine::getLogger().fatal()
        << createVulkanErrorMessage(resultCode, errorMessage, debugName);
    std::terminate();
  }
}

} // namespace quoll::rhi
