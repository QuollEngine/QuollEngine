#pragma once

#include "RenderHandle.h"
#include "AccessFlags.h"
#include "ImageLayout.h"
#include "StageFlags.h"

namespace quoll::rhi {

struct MemoryBarrier {
  Access srcAccess{Access::None};

  Access dstAccess{Access::None};

  PipelineStage srcStage{PipelineStage::None};

  PipelineStage dstStage{PipelineStage::None};
};

struct ImageBarrier {
  Access srcAccess{Access::None};

  Access dstAccess{Access::None};

  PipelineStage srcStage{PipelineStage::None};

  PipelineStage dstStage{PipelineStage::None};

  ImageLayout srcLayout{ImageLayout::Undefined};

  ImageLayout dstLayout{ImageLayout::Undefined};

  TextureHandle texture = TextureHandle::Null;

  u32 baseLevel = 0;

  u32 levelCount = 1;
};

struct BufferBarrier {
  Access srcAccess{Access::None};

  Access dstAccess{Access::None};

  PipelineStage srcStage{PipelineStage::None};

  PipelineStage dstStage{PipelineStage::None};

  BufferHandle buffer = BufferHandle::Null;

  u32 offset = 0;

  u32 size = 0;
};

} // namespace quoll::rhi
