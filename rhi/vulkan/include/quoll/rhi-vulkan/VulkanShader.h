#pragma once

#include "quoll/rhi/DescriptorLayoutDescription.h"
#include "quoll/rhi/ShaderDescription.h"
#include "VulkanDeviceObject.h"

namespace quoll::rhi {

class VulkanShader : NoCopyMove {
public:
  struct ReflectionData {
    std::vector<VkPushConstantRange> pushConstantRanges;

    std::map<u32, DescriptorLayoutDescription> descriptorLayouts;
  };

public:
  VulkanShader(const ShaderDescription &description,
               VulkanDeviceObject &device);

  ~VulkanShader();

  inline const VkShaderModule &getShaderModule() const { return mShaderModule; }

  inline const ReflectionData &getReflectionData() const {
    return mReflectionData;
  }

  inline const VkShaderStageFlagBits getShaderStage() const { return mStage; }

private:
  static std::vector<char> readShaderFile(const Path &filepath);

  void createReflectionInfo(const std::vector<char> &bytes);

private:
  VulkanDeviceObject &mDevice;

  VkShaderModule mShaderModule = VK_NULL_HANDLE;
  VkShaderStageFlagBits mStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

  ReflectionData mReflectionData{};
  Path mPath;
};

} // namespace quoll::rhi
