#pragma once

#include "RenderHandle.h"
#include "PipelineDescription.h"
#include "RenderPassDescription.h"
#include "RenderCommandList.h"

namespace liquid::rhi {

class RenderGraphEvaluator;
class RenderGraph;

/**
 * @brief Render graph attachment data
 */
struct AttachmentData {
  /**
   * Clear value
   */
  AttachmentClearValue clearValue;

  /**
   * Load operation
   */
  AttachmentLoadOp loadOp = AttachmentLoadOp::DontCare;

  /**
   * Store operation
   */
  AttachmentStoreOp storeOp = AttachmentStoreOp::DontCare;
};

/**
 * @brief Render graph pass
 */
class RenderGraphPass {
  using ExecutorFn = std::function<void(RenderCommandList &)>;
  friend RenderGraph;
  friend RenderGraphEvaluator;

public:
  /**
   * @brief Create render graph pass
   *
   * @param name Pass name
   */
  RenderGraphPass(const String &name);

  /**
   * @brief Execute pass
   *
   * @param commandList Command list
   */
  void execute(RenderCommandList &commandList);

  /**
   * @brief Set output texture
   *
   * @param handle Texture handle
   * @param clearValue Clear value
   */
  void write(TextureHandle handle, const AttachmentClearValue &clearValue);

  /**
   * @brief Set input texture
   *
   * @param handle Texture handle
   */
  void read(TextureHandle handle);

  /**
   * @brief Set executor function
   *
   * @param executor Executor function
   */
  void setExecutor(const ExecutorFn &executor);

  /**
   * @brief Add pipeline
   *
   * @param handle Pipeline handle
   */
  void addPipeline(PipelineHandle handle);

  /**
   * @brief Get pass name
   *
   * @return Pass name
   */
  inline const String &getName() const { return mName; }

  /**
   * @brief Get input textures
   *
   * @return Input textures
   */
  inline const std::vector<TextureHandle> &getInputs() const { return mInputs; }

  /**
   * @brief Get output textures
   *
   * @return Output textures
   */
  inline const std::vector<TextureHandle> &getOutputs() const {
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
  inline const std::vector<PipelineHandle> &getPipelines() const {
    return mPipelines;
  }

  /**
   * @brief Get framebuffers
   *
   * @return Framebuffers
   */
  inline const std::vector<FramebufferHandle> &getFramebuffers() const {
    return mFramebuffers;
  }

  /**
   * @brief Get dimensions
   *
   * @return Dimensions
   */
  inline const glm::uvec3 &getDimensions() const { return mDimensions; }

private:
  std::vector<AttachmentData> mAttachments;
  std::vector<TextureHandle> mOutputs;
  std::vector<TextureHandle> mInputs;
  std::vector<PipelineHandle> mPipelines;
  std::function<void(RenderCommandList &commandList)> mExecutor;

  rhi::RenderPassHandle mRenderPass = rhi::RenderPassHandle::Invalid;
  std::vector<FramebufferHandle> mFramebuffers;
  glm::uvec3 mDimensions{};

  String mName;
};

} // namespace liquid::rhi
