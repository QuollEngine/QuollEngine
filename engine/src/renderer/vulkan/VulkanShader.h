#pragma once

#include "core/Base.h"
#include <vulkan/vulkan.hpp>
#include "renderer/Shader.h"

namespace liquid {

class VulkanShader : public Shader {
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

private:
  /**
   * @brief Read shader file
   *
   * @param shaderFile Shader file
   * @return Shader data in bytes
   */
  static std::vector<char> readShaderFile(const String &shaderFile);

private:
  VkShaderModule shaderModule = nullptr;

  VkDevice device = nullptr;
};

} // namespace liquid
