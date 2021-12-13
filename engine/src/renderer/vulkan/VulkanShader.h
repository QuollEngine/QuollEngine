#pragma once

#include "core/Base.h"
#include <vulkan/vulkan.hpp>
#include "renderer/Shader.h"

#include "spirv_reflect.h"

namespace liquid {

class VulkanShader : public Shader {
  struct ReflectionData {
    std::vector<VkPushConstantRange> pushConstantRanges;
    std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayouts;
  };

public:
  /**
   * @brief Creates Vulkan shader
   *
   * @param device Vulkan device
   * @param shaderFile Path to shader
   */
  VulkanShader(VkDevice device, const String &shaderFile);

  /**
   * @brief Destroys shader
   */
  ~VulkanShader();

  VulkanShader(const VulkanShader &rhs) = delete;
  VulkanShader(VulkanShader &&rhs) = delete;
  VulkanShader &operator=(const VulkanShader &rhs) = delete;
  VulkanShader &operator=(VulkanShader &&rhs) = delete;

  /**
   * @brief Gets Vulkan shader
   *
   * @return Vulkan shader module
   */
  inline const VkShaderModule &getShaderModule() const { return shaderModule; }

  /**
   * @brief Get reflection data
   *
   * @return Reflection data
   */
  inline const ReflectionData &getReflectionData() const {
    return reflectionData;
  }

  /**
   * @brief Get shader stage
   *
   * @return Shader stage flag
   */
  inline const VkShaderStageFlagBits getShaderStage() const { return stage; }

private:
  /**
   * @brief Read shader file
   *
   * @param shaderFile Shader file
   * @return Shader data in bytes
   */
  static std::vector<char> readShaderFile(const String &shaderFile);

  /**
   * @brief Create reflection info
   *
   * @param bytes SpirV Bytes
   * @param shaderFile Shader filename
   */
  void createReflectionInfo(const std::vector<char> &bytes,
                            const String &shaderFile);

private:
  VkShaderModule shaderModule = VK_NULL_HANDLE;
  VkShaderStageFlagBits stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

  ReflectionData reflectionData{};

  VkDevice device = nullptr;
};

} // namespace liquid
