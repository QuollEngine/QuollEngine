#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/PipelineDescription.h"
#include "quoll/rhi/RenderPassDescription.h"
#include "quoll/rhi/RenderCommandList.h"
#include "quoll/rhi/BufferDescription.h"
#include "RenderGraphResource.h"

namespace quoll {

class RenderGraph;

enum class AttachmentType { Color, Depth, Resolve };

struct AttachmentData {
  AttachmentType type = AttachmentType::Color;

  rhi::AttachmentClearValue clearValue;

  rhi::AttachmentLoadOp loadOp = rhi::AttachmentLoadOp::DontCare;

  rhi::AttachmentStoreOp storeOp = rhi::AttachmentStoreOp::DontCare;
};

struct RenderTargetData {
  RenderGraphResource<rhi::TextureHandle> texture;

  rhi::ImageLayout srcLayout{rhi::ImageLayout::Undefined};

  rhi::ImageLayout dstLayout{rhi::ImageLayout::Undefined};
};

struct RenderGraphPassBufferData {
  rhi::BufferHandle buffer = rhi::BufferHandle::Null;

  rhi::BufferUsage usage = rhi::BufferUsage::None;
};

struct RenderGraphPassBarrier {
  std::vector<rhi::MemoryBarrier> memoryBarriers;

  std::vector<rhi::ImageBarrier> imageBarriers;

  std::vector<rhi::BufferBarrier> bufferBarriers;
};

enum class RenderGraphPassType { Graphics, Compute };

class RenderGraphPass {
  using ExecutorFn = std::function<void(rhi::RenderCommandList &, u32)>;
  friend RenderGraph;

public:
  RenderGraphPass(StringView name, RenderGraphPassType type);

  RenderGraphPass(const RenderGraphPass &rhs) = default;

  RenderGraphPass &operator=(const RenderGraphPass &rhs) = default;

  RenderGraphPass(RenderGraphPass &&rhs) = default;

  RenderGraphPass &operator=(RenderGraphPass &&rhs) = default;

  ~RenderGraphPass() = default;

  void execute(rhi::RenderCommandList &commandList, u32 frameIndex);

  void write(RenderGraphResource<rhi::TextureHandle> handle,
             AttachmentType type, const rhi::AttachmentClearValue &clearValue);

  void read(RenderGraphResource<rhi::TextureHandle> handle);

  void write(rhi::BufferHandle handle, rhi::BufferUsage usage);

  void read(rhi::BufferHandle handle, rhi::BufferUsage usage);

  void setExecutor(const ExecutorFn &executor);

  void addPipeline(rhi::PipelineHandle handle);

  inline const String &getName() const { return mName; }

  inline const RenderGraphPassType &getType() const { return mType; }

  inline const std::vector<RenderTargetData> &getTextureInputs() const {
    return mTextureInputs;
  }

  inline const std::vector<RenderTargetData> &getTextureOutputs() const {
    return mTextureOutputs;
  }

  inline const std::vector<RenderGraphPassBufferData> &getBufferInputs() const {
    return mBufferInputs;
  }

  inline rhi::RenderPassHandle getRenderPass() const { return mRenderPass; }

  inline rhi::FramebufferHandle getFramebuffer() const { return mFramebuffer; }

  inline const std::vector<rhi::PipelineHandle> &getPipelines() const {
    return mPipelines;
  }

  inline const std::vector<RenderGraphPassBufferData> &
  getBufferOutputs() const {
    return mBufferOutputs;
  }

  inline const std::vector<AttachmentData> &getAttachments() const {
    return mAttachments;
  }

  inline const glm::uvec3 &getDimensions() const { return mDimensions; }

  inline const RenderGraphPassBarrier &getSyncDependencies() const {
    return mDependencies;
  }

private:
  std::vector<AttachmentData> mAttachments;
  std::vector<RenderTargetData> mTextureOutputs;
  std::vector<RenderTargetData> mTextureInputs;

  std::vector<RenderGraphPassBufferData> mBufferInputs;
  std::vector<RenderGraphPassBufferData> mBufferOutputs;

  RenderGraphPassBarrier mDependencies;

  ExecutorFn mExecutor;

  std::vector<rhi::PipelineHandle> mPipelines;

  String mName;
  RenderGraphPassType mType;

  bool mCreated = false;

  // Graphics specific resources
  rhi::RenderPassHandle mRenderPass = rhi::RenderPassHandle::Null;
  rhi::FramebufferHandle mFramebuffer = rhi::FramebufferHandle::Null;
  glm::uvec3 mDimensions{};
};

} // namespace quoll
