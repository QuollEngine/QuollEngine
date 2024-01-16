#pragma once

#include "RenderGraphPass.h"

namespace quoll {

struct RenderGraphTextureSyncDependency {
  rhi::PipelineStage stage;

  rhi::Access access;

  rhi::ImageLayout layout;
};

struct RenderGraphBufferSyncDependency {
  rhi::PipelineStage stage;

  rhi::Access access;
};

class RenderGraphSyncDependency {
public:
  static RenderGraphTextureSyncDependency
  getTextureRead(RenderGraphPassType type);

  static RenderGraphTextureSyncDependency
  getTextureWrite(RenderGraphPassType type, AttachmentType attachmentType);

  static RenderGraphBufferSyncDependency getBufferRead(RenderGraphPassType type,
                                                       rhi::BufferUsage usage);

  static RenderGraphBufferSyncDependency
  getBufferWrite(RenderGraphPassType type);
};

} // namespace quoll
