#pragma once

#include "liquid/renderer/render-graph/RenderGraphPipelineDescription.h"
#include "liquid/renderer/render-graph/RenderGraphAttachmentDescription.h"
#include "liquid/rhi/Descriptor.h"

#include <vulkan/vulkan.hpp>

namespace liquid {

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
};

} // namespace liquid
