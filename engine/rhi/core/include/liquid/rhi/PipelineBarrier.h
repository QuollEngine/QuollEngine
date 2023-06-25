#pragma once

#include "RenderHandle.h"
#include "AccessFlags.h"
#include "ImageLayout.h"

namespace liquid::rhi {

/**
 * @brief Memory barrier
 */
struct MemoryBarrier {
  /**
   * Source access flags
   */
  Access srcAccess{Access::None};

  /**
   * Destination access flags
   */
  Access dstAccess{Access::None};
};

/**
 * @brief Image barrier
 */
struct ImageBarrier {
  /**
   * Source access flags
   */
  Access srcAccess{Access::None};

  /**
   * Destination access flags
   */
  Access dstAccess{Access::None};

  /**
   * Source image layout
   */
  ImageLayout srcLayout{ImageLayout::Undefined};

  /**
   * Destination image layout
   */
  ImageLayout dstLayout{ImageLayout::Undefined};

  /**
   * Texture
   */
  TextureHandle texture = TextureHandle::Null;

  /**
   * Base mip level
   */
  uint32_t baseLevel = 0;

  /**
   * Mip level count
   */
  uint32_t levelCount = 1;
};

/**
 * @brief Buffer barrier
 */
struct BufferBarrier {
  /**
   * Source access flags
   */
  Access srcAccess{Access::None};

  /**
   * Destination access flags
   */
  Access dstAccess{Access::None};

  /**
   * Buffer handle
   */
  BufferHandle buffer = BufferHandle::Null;

  /**
   * Buffer offset
   */
  uint32_t offset = 0;

  /**
   * Buffer size
   */
  uint32_t size = 0;
};

} // namespace liquid::rhi
