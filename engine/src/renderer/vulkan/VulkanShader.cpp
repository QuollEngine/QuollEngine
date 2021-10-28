#include "core/Base.h"
#include "core/EngineGlobals.h"
#include "VulkanShader.h"
#include "VulkanError.h"

namespace liquid {

std::vector<char> VulkanShader::readShaderFile(const String &shaderFile) {
  std::ifstream file(shaderFile, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw VulkanError("Failed to open shader file \"" + shaderFile + "\"",
                      VK_SUCCESS);
  }

  std::streamsize fileSize = file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

VulkanShader::VulkanShader(VkDevice device_, const String &shaderFile)
    : device(device_) {
  const auto &shaderBytes = VulkanShader::readShaderFile(shaderFile);

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pNext = nullptr;

  createInfo.codeSize = shaderBytes.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(shaderBytes.data());

  checkForVulkanError(
      vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule),
      "Failed to create shader module from \"" + shaderFile + "\"");

  LOG_DEBUG("[Vulkan] Shader module created from \"" + shaderFile + "\"");
}

VulkanShader::~VulkanShader() {
  if (shaderModule) {
    vkDestroyShaderModule(device, shaderModule, nullptr);
    shaderModule = VK_NULL_HANDLE;
    LOG_DEBUG("[Vulkan] Shader module destroyed");
  }
}

} // namespace liquid
