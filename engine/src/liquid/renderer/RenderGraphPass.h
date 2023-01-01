#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/PipelineDescription.h"
#include "liquid/rhi/RenderPassDescription.h"
#include "liquid/rhi/RenderCommandList.h"

#include "RenderGraphRegistry.h"

namespace liquid {

class RenderGraph;
class RenderGraphEvaluator;

/**
 * @brief Render graph attachment data
 */
struct AttachmentData {
  /**
   * Clear value
   */
  rhi::AttachmentClearValue clearValue;

  /**
   * Load operation
   */
  rhi::AttachmentLoadOp loadOp = rhi::AttachmentLoadOp::DontCare;

  /**
   * Store operation
   */
  rhi::AttachmentStoreOp storeOp = rhi::AttachmentStoreOp::DontCare;
};

/**
 * @brief Render target data
 */
struct RenderTargetData {
  /**
   * Texture
   */
  rhi::TextureHandle texture = rhi::TextureHandle::Invalid;

  /**
   * Source image layout
   */
  rhi::ImageLayout srcLayout{rhi::ImageLayout::Undefined};

  /**
   * Destination image layout
   */
  rhi::ImageLayout dstLayout{rhi::ImageLayout::Undefined};
};

/**
 * @brief Render graph pass barrier
 */
struct RenderGraphPassBarrier {
  /**
   * Barrier active
   */
  bool enabled = false;

  /**
   * Source pipeline stage
   */
  rhi::PipelineStage srcStage{rhi::PipelineStage::None};

  /**
   * Destination pipeline stage
   */
  rhi::PipelineStage dstStage{rhi::PipelineStage::None};

  /**
   * Memory barriers
   */
  std::vector<rhi::MemoryBarrier> memoryBarriers;

  /**
   * Image barriers
   */
  std::vector<rhi::ImageBarrier> imageBarriers;
};

/**
 * @brief Render graph pass
 */
class RenderGraphPass {
  using ExecutorFn = std::function<void(rhi::RenderCommandList &,
                                        const RenderGraphRegistry &, uint32_t)>;
  friend RenderGraph;
  friend RenderGraphEvaluator;

public:
  /**
   * @brief Create render graph pass
   *
   * @param name Pass name
   */
  RenderGraphPass(StringView name);

  /**
   * @brief Copy another render pass into this
   *
   * @param rhs Render pass to copy
   */
  RenderGraphPass(const RenderGraphPass &rhs) = default;

  /**
   * @brief Copy another render pass into this
   *
   * @param rhs Render pass to copy
   * @return This render pass
   */
  RenderGraphPass &operator=(const RenderGraphPass &rhs) = default;

  /**
   * @brief Move another render pass into this
   *
   * @param rhs Render pass to move
   */
  RenderGraphPass(RenderGraphPass &&rhs) = default;

  /**
   * @brief Move another render pass into this
   *
   * @param rhs Render pass to move
   * @return This render pass
   */
  RenderGraphPass &operator=(RenderGraphPass &&rhs) = default;

  /**
   * @brief Destroy render pass
   */
  ~RenderGraphPass() = default;

  /**
   * @brief Execute pass
   *
   * @param commandList Command list
   * @param frameIndex Frame index
   */
  void execute(rhi::RenderCommandList &commandList, uint32_t frameIndex);

  /**
   * @brief Set output texture
   *
   * @param handle Texture handle
   * @param clearValue Clear value
   */
  void write(rhi::TextureHandle handle,
             const rhi::AttachmentClearValue &clearValue);

  /**
   * @brief Set input texture
   *
   * @param handle Texture handle
   */
  void read(rhi::TextureHandle handle);

  /**
   * @brief Set executor function
   *
   * @param executor Executor function
   */
  void setExecutor(const ExecutorFn &executor);

  /**
   * @brief Add pipeline
   *
   * @param description Pipeline description
   * @return Virtual pipeline handle
   */
  VirtualPipelineHandle
  addPipeline(const rhi::PipelineDescription &description);

  /**
   * @brief Get pass name
   *
   * @return Pass name
   */
  inline const String &getName() const { return mName; }

  /**
   * @brief Get input textures
   *
   * @return Input render targets
   */
  inline const std::vector<RenderTargetData> &getInputs() const {
    return mInputs;
  }

  /**
   * @brief Get output textures
   *
   * @return Output render targets
   */
  inline const std::vector<RenderTargetData> &getOutputs() const {
    return mOutputs;
  }

  /**
   * @brief Get attachment data
   *
   * @return Attachment data
   */
  inline const std::vector<AttachmentData> &getAttachments() const {
    return mAttachments;
  }

  /**
   * @brief Get pipelines
   *
   * @return Pipelines
   */
  inline const std::vector<rhi::PipelineHandle> &getPipelines() const {
    return mPipelines;
  }

  /**
   * @brief Get framebuffer
   *
   * @return Framebuffer
   */
  inline rhi::FramebufferHandle getFramebuffer() const { return mFramebuffer; }

  /**
   * @brief Get dimensions
   *
   * @return Dimensions
   */
  inline const glm::uvec3 &getDimensions() const { return mDimensions; }

  /**
   * @brief Get pass pre barrier
   *
   * @return Pass pre barrier
   */
  inline const RenderGraphPassBarrier &getPreBarrier() const {
    return mPreBarrier;
  }

  /**
   * @brief Get pass pre barrier
   *
   * @return Pass pre barrier
   */
  inline const RenderGraphPassBarrier &getPostBarrier() const {
    return mPostBarrier;
  }

private:
  std::vector<AttachmentData> mAttachments;
  std::vector<RenderTargetData> mOutputs;
  std::vector<RenderTargetData> mInputs;
  std::vector<rhi::PipelineHandle> mPipelines;

  RenderGraphPassBarrier mPreBarrier;
  RenderGraphPassBarrier mPostBarrier;

  ExecutorFn mExecutor;

  rhi::RenderPassHandle mRenderPass = rhi::RenderPassHandle::Invalid;
  rhi::FramebufferHandle mFramebuffer = rhi::FramebufferHandle::Invalid;
  glm::uvec3 mDimensions{};

  RenderGraphRegistry mRegistry;

  String mName;
};

} // namespace liquid
