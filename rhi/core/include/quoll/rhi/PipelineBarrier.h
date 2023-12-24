#pragma once

#include "RenderHandle.h"
#include "AccessFlags.h"
#include "ImageLayout.h"
#include "StageFlags.h"

namespace quoll::rhi {

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

  /**
   * Source pipeline stage
   */
  PipelineStage srcStage{PipelineStage::None};

  /**
   * Destination pipeline stage
   */
  PipelineStage dstStage{PipelineStage::None};
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
   * Source pipeline stage
   */
  PipelineStage srcStage{PipelineStage::None};

  /**
   * Destination pipeline stage
   */
  PipelineStage dstStage{PipelineStage::None};

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
  u32 baseLevel = 0;

  /**
   * Mip level count
   */
  u32 levelCount = 1;
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
   * Source pipeline stage
   */
  PipelineStage srcStage{PipelineStage::None};

  /**
   * Destination pipeline stage
   */
  PipelineStage dstStage{PipelineStage::None};

  /**
   * Buffer handle
   */
  BufferHandle buffer = BufferHandle::Null;

  /**
   * Buffer offset
   */
  u32 offset = 0;

  /**
   * Buffer size
   */
  u32 size = 0;
};

} // namespace quoll::rhi
