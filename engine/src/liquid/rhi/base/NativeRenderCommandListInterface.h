#pragma once

#include "liquid/rhi/Descriptor.h"
#include "liquid/rhi/RenderHandle.h"

#include <vulkan/vulkan.hpp>

namespace liquid::rhi {

class NativeRenderCommandListInterface {
public:
  /**
   * @brief Begin render pass
   *
   * @param renderPass Render pass
   * @param framebuffer Framebuffer
   * @param renderAreaOffset Render area offset
   * @param renderAreaSize Render area size
   */
  virtual void beginRenderPass(rhi::RenderPassHandle renderPass,
                               FramebufferHandle framebuffer,
                               const glm::ivec2 &renderAreaOffset,
                               const glm::uvec2 &renderAreaSize) = 0;

  /**
   * @brief End render pass
   */
  virtual void endRenderPass() = 0;

  /**
   * @brief Bind pipeline
   *
   * @param pipeline Pipeline
   */
  virtual void bindPipeline(PipelineHandle pipeline) = 0;

  /**
   * @brief Bind descriptor
   *
   * @param pipeline Pipeline
   * @param firstSet First set
   * @param descriptor Descriptor
   */
  virtual void bindDescriptor(PipelineHandle pipeline, uint32_t firstSet,
                              const Descriptor &descriptor) = 0;

  /**
   * @brief Bind vertex buffer
   *
   * @param buffer Vertex buffer
   */
  virtual void bindVertexBuffer(BufferHandle buffer) = 0;

  /**
   * @brief Bind index buffer
   *
   * @param buffer Index buffer
   * @param indexType Index buffer data type
   */
  virtual void bindIndexBuffer(BufferHandle buffer, VkIndexType indexType) = 0;

  /**
   * @brief Push constants
   *
   * @param pipeline Pipeline
   * @param stageFlags Stage flags
   * @param offset Offset
   * @param size Size
   * @param data Data
   */
  virtual void pushConstants(PipelineHandle pipeline,
                             VkShaderStageFlags stageFlags, uint32_t offset,
                             uint32_t size, void *data) = 0;

  /**
   * @brief Draw
   *
   * @param vertexCount Vertex count
   * @param firstVertex First vertex
   */
  virtual void draw(uint32_t vertexCount, uint32_t firstVertex) = 0;

  /**
   * @brief Draw indexed
   *
   * @param indexCount Index count
   * @param firstIndex Offset of first index
   * @param vertexOffset Vertex offset
   */
  virtual void drawIndexed(uint32_t indexCount, uint32_t firstIndex,
                           int32_t vertexOffset) = 0;

  /**
   * @brief Set viewport
   *
   * @param offset Viewport offset
   * @param size Viewport size
   * @param depthRange Viewport depth range
   */
  virtual void setViewport(const glm::vec2 &offset, const glm::vec2 &size,
                           const glm::vec2 &depthRange) = 0;

  /**
   * @brief Set scissor
   *
   * @param offset Scissor offset
   * @param size Scissor size
   */
  virtual void setScissor(const glm::ivec2 &offset, const glm::uvec2 &size) = 0;
};

} // namespace liquid::rhi
