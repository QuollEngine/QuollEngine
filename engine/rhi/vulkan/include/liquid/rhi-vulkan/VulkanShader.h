#pragma once

#include <vulkan/vulkan.hpp>

#include "liquid/rhi/ShaderDescription.h"
#include "VulkanDeviceObject.h"

namespace liquid::rhi {

/**
 * @brief Vulkan shader
 */
class VulkanShader {
public:
  /**
   * @brief Shader reflection descriptor set layout data
   */
  struct ReflectionDescriptorSetLayout {
    /**
     * Binding names
     */
    std::vector<String> names;

    /**
     * Bindings
     */
    std::vector<VkDescriptorSetLayoutBinding> bindings;
  };

  /**
   * @brief Shader reflection data
   */
  struct ReflectionData {
    /**
     * Push constant ranges
     */
    std::vector<VkPushConstantRange> pushConstantRanges;

    /**
     * Descriptor set layouts
     */
    std::map<uint32_t, ReflectionDescriptorSetLayout> descriptorSetLayouts;
  };

public:
  /**
   * @brief Creates Vulkan shader
   *
   * @param description Shader description
   * @param device Vulkan device
   */
  VulkanShader(const ShaderDescription &description,
               VulkanDeviceObject &device);

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
  inline const VkShaderModule &getShaderModule() const { return mShaderModule; }

  /**
   * @brief Get reflection data
   *
   * @return Reflection data
   */
  inline const ReflectionData &getReflectionData() const {
    return mReflectionData;
  }

  /**
   * @brief Get shader stage
   *
   * @return Shader stage flag
   */
  inline const VkShaderStageFlagBits getShaderStage() const { return mStage; }

private:
  /**
   * @brief Read shader file
   *
   * @param filepath File path
   * @return Shader data in bytes
   */
  static std::vector<char> readShaderFile(const Path &filepath);

  /**
   * @brief Create reflection info
   *
   * @param bytes SpirV Bytes
   */
  void createReflectionInfo(const std::vector<char> &bytes);

private:
  VulkanDeviceObject &mDevice;

  VkShaderModule mShaderModule = VK_NULL_HANDLE;
  VkShaderStageFlagBits mStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

  ReflectionData mReflectionData{};
  Path mPath;
};

} // namespace liquid::rhi
