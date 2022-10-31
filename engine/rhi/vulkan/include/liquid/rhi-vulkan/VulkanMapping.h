#pragma once

#include "liquid/rhi/RenderPassDescription.h"
#include "liquid/rhi/PipelineDescription.h"
#include "liquid/rhi/Descriptor.h"
#include "liquid/rhi/AccessFlags.h"
#include "liquid/rhi/ImageLayout.h"
#include "liquid/rhi/StageFlags.h"
#include "liquid/rhi/IndexType.h"
#include "liquid/rhi/Format.h"

#include <vulkan/vulkan.hpp>

namespace liquid::rhi {

/**
 * @brief Vulkan mapper
 *
 * Maps engine specific types
 * with Vulkan types
 */
class VulkanMapping {
public:
  /**
   * @brief Get Vulkan primitive topology
   *
   * @param topology Primitive topology
   * @return Vulkan primitive topology
   */
  static VkPrimitiveTopology getPrimitiveTopology(PrimitiveTopology topology);

  /**
   * @brief Get Vulkan load operation
   *
   * @param loadOp Load operation
   * @return Vulkan load operation
   */
  static VkAttachmentLoadOp getAttachmentLoadOp(AttachmentLoadOp loadOp);

  /**
   * @brief Get Vulkan store operation
   *
   * @param storeOp Store operation
   * @return Vulkan store operation
   */
  static VkAttachmentStoreOp getAttachmentStoreOp(AttachmentStoreOp storeOp);

  /**
   * @brief Get Vulkan polygon mode
   *
   * @param polygonMode Polygon mode
   * @return Vulkan polygon mode
   */
  static VkPolygonMode getPolygonMode(PolygonMode polygonMode);

  /**
   * @brief Get Vulkan cull mode
   *
   * @param cullMode Cull mode
   * @return Vulkan cull mode
   */
  static VkCullModeFlags getCullMode(CullMode cullMode);

  /**
   * @brief Get Vulkan front face
   *
   * @param frontFace Front face
   * @return Vulkan front face
   */
  static VkFrontFace getFrontFace(FrontFace frontFace);

  /**
   * @brief Get Vulkan blend factor
   *
   * @param blendFactor Blend factor
   * @return Vulkan blend factor
   */
  static VkBlendFactor getBlendFactor(BlendFactor blendFactor);

  /**
   * @brief Get Vulkan blend operation
   *
   * @param blendOp Blend operation
   * @return Vulkan blend operation
   */
  static VkBlendOp getBlendOp(BlendOp blendOp);

  /**
   * @brief Get Vulkan vertex input rate
   *
   * @param vertexInputRate Vertex input rate
   * @return Vulkan vertex input rate
   */
  static VkVertexInputRate getVertexInputRate(VertexInputRate vertexInputRate);

  /**
   * @brief Get Vulkan descriptor type
   *
   * @param descriptorType Descriptor type
   * @return Vulkan descriptor type
   */
  static VkDescriptorType getDescriptorType(DescriptorType descriptorType);

  /**
   * @brief Get Vulkan access flags
   *
   * @param access Access
   * @return Vulkan access flags
   */
  static VkAccessFlags getAccessFlags(Access access);

  /**
   * @brief Get Vulkan image layout
   *
   * @param imageLayout Image layout
   * @return Vulkan image layout
   */
  static VkImageLayout getImageLayout(ImageLayout imageLayout);

  /**
   * @brief Get Vulkan pipeline bind point
   *
   * @param pipelineBindPoint Pipeline bind point
   * @return Vulkan pipeline bind point
   */
  static VkPipelineBindPoint
  getPipelineBindPoint(PipelineBindPoint pipelineBindPoint);

  /**
   * @brief Get Vulkan shader stage flags
   *
   * @param shaderStage Shader stage
   * @return Vulkan shader stage flags
   */
  static VkShaderStageFlags getShaderStageFlags(ShaderStage shaderStage);

  /**
   * @brief Get Vulkan pipeline stage flags
   *
   * @param pipelineStage Pipeline stage
   * @return Vulkan pipeline stage flags
   */
  static VkPipelineStageFlags
  getPipelineStageFlags(PipelineStage pipelineStage);

  /**
   * @brief Get Vulkan index type
   *
   * @param indexType Index type
   * @return Vulkan index type
   */
  static VkIndexType getIndexType(IndexType indexType);

  /**
   * @brief Get Vulkan format
   *
   * @param format Format
   * @return Vulkan format
   */
  static VkFormat getFormat(Format format);
};

} // namespace liquid::rhi
