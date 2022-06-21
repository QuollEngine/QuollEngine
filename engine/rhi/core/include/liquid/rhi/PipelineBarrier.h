#pragma once

#include <vulkan/vulkan.h>
#include "RenderHandle.h"

namespace liquid::rhi {

/**
 * @brief Memory barrier
 */
struct MemoryBarrier {
  /**
   * @brief Source access flags
   */
  VkAccessFlags srcAccess = VK_ACCESS_NONE_KHR;

  /**
   * @brief Destination access flags
   */
  VkAccessFlags dstAccess = VK_ACCESS_NONE_KHR;
};

/**
 * @brief Image barrier
 */
struct ImageBarrier {
  /**
   * @brief Source access flags
   */
  VkAccessFlags srcAccess = VK_ACCESS_NONE_KHR;

  /**
   * @brief Destination access flags
   */
  VkAccessFlags dstAccess = VK_ACCESS_NONE_KHR;

  /**
   * @brief Source image layout
   */
  VkImageLayout srcLayout = VK_IMAGE_LAYOUT_MAX_ENUM;

  /**
   * @brief Destination image layout
   */
  VkImageLayout dstLayout = VK_IMAGE_LAYOUT_MAX_ENUM;

  /**
   * @brief Texture
   */
  TextureHandle texture = TextureHandle::Invalid;
};

} // namespace liquid::rhi
