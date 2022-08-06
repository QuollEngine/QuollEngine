#pragma once

#include <vulkan/vulkan.h>
#include "RenderHandle.h"

namespace liquid::rhi {

/**
 * @brief Memory barrier
 */
struct MemoryBarrier {
  /**
   * Source access flags
   */
  VkAccessFlags srcAccess = VK_ACCESS_NONE_KHR;

  /**
   * Destination access flags
   */
  VkAccessFlags dstAccess = VK_ACCESS_NONE_KHR;
};

/**
 * @brief Image barrier
 */
struct ImageBarrier {
  /**
   * Source access flags
   */
  VkAccessFlags srcAccess = VK_ACCESS_NONE_KHR;

  /**
   * Destination access flags
   */
  VkAccessFlags dstAccess = VK_ACCESS_NONE_KHR;

  /**
   * Source image layout
   */
  VkImageLayout srcLayout = VK_IMAGE_LAYOUT_MAX_ENUM;

  /**
   * Destination image layout
   */
  VkImageLayout dstLayout = VK_IMAGE_LAYOUT_MAX_ENUM;

  /**
   * Texture
   */
  TextureHandle texture = TextureHandle::Invalid;
};

} // namespace liquid::rhi
