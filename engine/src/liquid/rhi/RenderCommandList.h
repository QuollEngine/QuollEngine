#pragma once

#include "base/NativeRenderCommandListInterface.h"

namespace liquid::rhi {

class RenderCommandList {
public:
  /**
   * @brief Default constructor
   */
  RenderCommandList() = default;

  /**
   * @brief Default destructor
   */
  ~RenderCommandList() = default;

  /**
   * @brief Create render command list
   *
   * @param nativeRenderCommandList Native render command list
   */
  RenderCommandList(NativeRenderCommandListInterface *nativeCommandBuffer)
      : mNativeRenderCommandList(nativeCommandBuffer) {}

  /**
   * @brief Move constructor
   *
   * @param rhs Other command list
   */
  RenderCommandList(RenderCommandList &&rhs) noexcept {
    mNativeRenderCommandList = std::move(rhs.mNativeRenderCommandList);
  }

  /**
   * @brief Move equality operator
   *
   * @param rhs Other render command list
   * @return This render command list
   */
  RenderCommandList &operator=(RenderCommandList &&rhs) noexcept {
    mNativeRenderCommandList = std::move(rhs.mNativeRenderCommandList);
    return *this;
  }

  RenderCommandList(const RenderCommandList &) = delete;
  RenderCommandList &operator=(const RenderCommandList &) = delete;

  /**
   * @brief Get native render command list
   *
   * @return Native render command list
   */
  inline const std::unique_ptr<NativeRenderCommandListInterface> &
  getNativeRenderCommandList() const {
    return mNativeRenderCommandList;
  }

  /**
   * @brief Begin render pass
   *
   * @param renderPass Render pass
   * @param framebuffer Framebuffer
   * @param renderAreaOffset Render area offset
   * @param renderAreaSize Render area size
   * @param clearValues Clear values
   */
  inline void beginRenderPass(rhi::RenderPassHandle renderPass,
                              FramebufferHandle framebuffer,
                              const glm::ivec2 &renderAreaOffset,
                              const glm::uvec2 &renderAreaSize,
                              const std::vector<VkClearValue> &clearValues) {
    mNativeRenderCommandList->beginRenderPass(
        renderPass, framebuffer, renderAreaOffset, renderAreaSize, clearValues);
  }

  /**
   * @brief End render pass
   */
  inline void endRenderPass() { mNativeRenderCommandList->endRenderPass(); }

  /**
   * @brief Bind pipeline
   *
   * @param pipeline Pipeline
   */
  void bindPipeline(PipelineHandle pipeline) {
    mNativeRenderCommandList->bindPipeline(pipeline);
  }

  /**
   * @brief Bind descriptor
   *
   * @param pipeline Pipeline
   * @param firstSet First set
   * @param descriptor Descriptor
   */
  inline void bindDescriptor(PipelineHandle pipeline, uint32_t firstSet,
                             const Descriptor &descriptor) {
    mNativeRenderCommandList->bindDescriptor(pipeline, firstSet, descriptor);
  }

  /**
   * @brief Bind vertex buffer
   *
   * @param buffer Vertex buffer
   */
  void bindVertexBuffer(BufferHandle buffer) {
    mNativeRenderCommandList->bindVertexBuffer(buffer);
  }

  /**
   * @brief Bind index buffer
   *
   * @param buffer Index buffer
   * @param indexType Index buffer data type
   */
  void bindIndexBuffer(BufferHandle buffer, VkIndexType indexType) {
    mNativeRenderCommandList->bindIndexBuffer(buffer, indexType);
  }

  /**
   * @brief Push constants
   *
   * @param pipeline Pipeline
   * @param stageFlags Stage flags
   * @param offset Offset
   * @param size Size
   * @param data Data
   */
  void pushConstants(PipelineHandle pipeline, VkShaderStageFlags stageFlags,
                     uint32_t offset, uint32_t size, void *data) {
    mNativeRenderCommandList->pushConstants(pipeline, stageFlags, offset, size,
                                            data);
  }

  /**
   * @brief Draw
   *
   * @param vertexCount Vertex count
   * @param firstVertex First vertex
   */
  void draw(uint32_t vertexCount, uint32_t firstVertex) {
    mNativeRenderCommandList->draw(vertexCount, firstVertex);
  }

  /**
   * @brief Draw indexed
   *
   * @param indexCount Index count
   * @param firstIndex Offset of first index
   * @param vertexOffset Vertex offset
   */
  void drawIndexed(uint32_t indexCount, uint32_t firstIndex,
                   int32_t vertexOffset) {
    mNativeRenderCommandList->drawIndexed(indexCount, firstIndex, vertexOffset);
  }

  /**
   * @brief Set viewport
   *
   * @param offset Viewport offset
   * @param size Viewport size
   * @param depthRange Viewport depth range
   */
  void setViewport(const glm::vec2 &offset, const glm::vec2 &size,
                   const glm::vec2 &depthRange) {
    mNativeRenderCommandList->setViewport(offset, size, depthRange);
  }

  /**
   * @brief Set scissor
   *
   * @param offset Scissor offset
   * @param size Scissor size
   */
  void setScissor(const glm::ivec2 &offset, const glm::uvec2 &size) {
    mNativeRenderCommandList->setScissor(offset, size);
  }

private:
  std::unique_ptr<NativeRenderCommandListInterface> mNativeRenderCommandList;
};

} // namespace liquid::rhi

namespace liquid::rhi {

using RenderCommandList = rhi::RenderCommandList;

} // namespace liquid::rhi
