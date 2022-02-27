#pragma once

#include <vulkan/vulkan.hpp>

#include "liquid/scene/Vertex.h"

namespace liquid {

/**
 * @brief Vertex Input Description for Vulkan
 *
 * Needed to determine bindings, attributes, and flags
 * for each structure
 */
struct VulkanVertexInputDescription {
  std::vector<VkVertexInputBindingDescription> bindings;
  std::vector<VkVertexInputAttributeDescription> attributes;

  VkPipelineVertexInputStateCreateFlags flags = 0;
};

/**
 * @brief Gets vertex input description for any type
 *
 * This function is treated as a template declaration
 * and must be specialized for any vertex data structure
 * that needs to be supported
 *
 * @tvalue VertexType Vertex Data Structure
 * @return Empty vertex input description object
 */
template <class VertexType>
VulkanVertexInputDescription getVulkanVertexInputDescription() {
  return VulkanVertexInputDescription{};
}

/**
 * @brief Vertex input description template specialization for Vertex data
 * structure
 *
 * @return Vertex input description for Vertex data structure
 */
template <>
inline VulkanVertexInputDescription getVulkanVertexInputDescription<Vertex>() {
  VulkanVertexInputDescription description{};

  const uint32_t POSITION_LOCATION = 0;
  const uint32_t NORMAL_LOCATION = 1;
  const uint32_t TANGENT_LOCATION = 2;
  const uint32_t COLOR_LOCATION = 3;
  const uint32_t TEXCOORD0_LOCATION = 4;
  const uint32_t TEXCOORD1_LOCATION = 5;

  VkVertexInputBindingDescription mainBinding = {};
  mainBinding.binding = 0;
  mainBinding.stride = sizeof(Vertex);
  mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  description.bindings.push_back(mainBinding);

  // (location = 0) Position
  VkVertexInputAttributeDescription positionAttribute = {};
  positionAttribute.binding = 0;
  positionAttribute.location = POSITION_LOCATION;
  positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
  positionAttribute.offset = offsetof(Vertex, x);

  // (location = 1) Normal
  VkVertexInputAttributeDescription normalAttribute = {};
  normalAttribute.binding = 0;
  normalAttribute.location = NORMAL_LOCATION;
  normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
  normalAttribute.offset = offsetof(Vertex, nx);

  // (location = 2) Tangent
  VkVertexInputAttributeDescription tangentAttribute{};
  tangentAttribute.binding = 0;
  tangentAttribute.location = TANGENT_LOCATION;
  tangentAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
  tangentAttribute.offset = offsetof(Vertex, tx);

  // (location = 3) Color
  VkVertexInputAttributeDescription colorAttribute = {};
  colorAttribute.binding = 0;
  colorAttribute.location = COLOR_LOCATION;
  colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
  colorAttribute.offset = offsetof(Vertex, r);

  // (location = 4) UV Coordinates 0
  VkVertexInputAttributeDescription texCoordAttribute0{};
  texCoordAttribute0.binding = 0;
  texCoordAttribute0.location = TEXCOORD0_LOCATION;
  texCoordAttribute0.format = VK_FORMAT_R32G32_SFLOAT;
  texCoordAttribute0.offset = offsetof(Vertex, u0);

  // (location = 5) UV Coordinates 1
  VkVertexInputAttributeDescription texCoordAttribute1{};
  texCoordAttribute1.binding = 0;
  texCoordAttribute1.location = TEXCOORD1_LOCATION;
  texCoordAttribute1.format = VK_FORMAT_R32G32_SFLOAT;
  texCoordAttribute1.offset = offsetof(Vertex, u1);

  description.attributes.push_back(positionAttribute);
  description.attributes.push_back(normalAttribute);
  description.attributes.push_back(tangentAttribute);
  description.attributes.push_back(colorAttribute);
  description.attributes.push_back(texCoordAttribute0);
  description.attributes.push_back(texCoordAttribute1);

  return description;
}

} // namespace liquid
