#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "VulkanShader.h"
#include "VulkanError.h"

#include "spirv_reflect.h"

namespace liquid::rhi {

VulkanShader::VulkanShader(const ShaderDescription &description,
                           VulkanDeviceObject &device)
    : mDevice(device), mPath(description.path) {
  const auto &shaderBytes = VulkanShader::readShaderFile(description.path);

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pNext = nullptr;

  createInfo.codeSize = shaderBytes.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(shaderBytes.data());

  checkForVulkanError(
      vkCreateShaderModule(device, &createInfo, nullptr, &mShaderModule),
      "Failed to create shader module from \"" + mPath.filename().string() +
          "\"");

  Engine::getLogger().info()
      << "Shader loaded: \"" +
             std::filesystem::relative(mPath, std::filesystem::current_path())
                 .string() +
             "\"";

  createReflectionInfo(shaderBytes);
}

VulkanShader::~VulkanShader() {
  if (mShaderModule) {
    vkDestroyShaderModule(mDevice, mShaderModule, nullptr);

    Engine::getLogger().info()
        << "Shader unloaded: \"" +
               std::filesystem::relative(mPath, std::filesystem::current_path())
                   .string() +
               "\"";
  }
}

std::vector<char> VulkanShader::readShaderFile(const Path &filepath) {
  std::ifstream file(filepath, std::ios::ate | std::ios::binary);

  LIQUID_ASSERT(file.is_open(),
                "Failed to open shader file \"" + filepath.string() + "\"");

  std::streamsize fileSize = file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}

void VulkanShader::createReflectionInfo(const std::vector<char> &bytes) {
  SpvReflectShaderModule shaderReflectModule;
  SpvReflectResult result = spvReflectCreateShaderModule(
      bytes.size(), bytes.data(), &shaderReflectModule);

  LIQUID_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS,
                "Failed to read reflection data from shader " +
                    mPath.filename().string());

  mStage = static_cast<VkShaderStageFlagBits>(shaderReflectModule.shader_stage);

  // Push constants
  {
    uint32_t count = 0;
    spvReflectEnumeratePushConstantBlocks(&shaderReflectModule, &count,
                                          nullptr);

    if (count > 0) {
      std::vector<SpvReflectBlockVariable *> blocks(count, nullptr);
      spvReflectEnumeratePushConstantBlocks(&shaderReflectModule, &count,
                                            &blocks.at(0));

      mReflectionData.pushConstantRanges.reserve(count);

      for (auto &blk : blocks) {
        const SpvReflectBlockVariable &reflectBlock = *blk;
        VkPushConstantRange range{};
        range.offset = reflectBlock.offset;
        range.size = reflectBlock.size;
        range.stageFlags = mStage;

        mReflectionData.pushConstantRanges.push_back(range);
      }
    }
  }

  // Descriptor layouts
  {
    uint32_t count = 0;
    spvReflectEnumerateDescriptorSets(&shaderReflectModule, &count, nullptr);

    if (count > 0) {
      std::vector<SpvReflectDescriptorSet *> descriptors(count, nullptr);
      spvReflectEnumerateDescriptorSets(&shaderReflectModule, &count,
                                        &descriptors.at(0));

      for (auto &ds : descriptors) {
        const SpvReflectDescriptorSet &reflectDescriptorSet = *ds;

        std::vector<VkDescriptorSetLayoutBinding> bindings(
            reflectDescriptorSet.binding_count);

        for (uint32_t i = 0; i < reflectDescriptorSet.binding_count; ++i) {
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
          auto *reflectBinding = reflectDescriptorSet.bindings[i];

          bindings.at(i).binding = reflectBinding->binding;
          bindings.at(i).stageFlags = mStage;
          bindings.at(i).descriptorType =
              static_cast<VkDescriptorType>(reflectBinding->descriptor_type);

          bindings.at(i).descriptorCount = 1;
          for (uint32_t j = 0; j < reflectBinding->array.dims_count; ++j) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            bindings.at(i).descriptorCount *= reflectBinding->array.dims[j];
          }
        }
        mReflectionData.descriptorSetLayouts.insert({ds->set, bindings});
      }
    }
  }

  spvReflectDestroyShaderModule(&shaderReflectModule);
}

} // namespace liquid::rhi
