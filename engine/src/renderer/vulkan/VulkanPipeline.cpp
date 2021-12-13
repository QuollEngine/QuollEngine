#include "core/Base.h"
#include "core/EngineGlobals.h"
#include "VulkanPipeline.h"

namespace liquid {

VulkanPipeline::VulkanPipeline(
    VkDevice device_, VkPipeline pipeline_, VkPipelineLayout pipelineLayout_,
    const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts_)
    : device(device_), pipeline(pipeline_), pipelineLayout(pipelineLayout_),
      descriptorSetLayouts(descriptorSetLayouts_) {}

VulkanPipeline::~VulkanPipeline() {
  if (pipeline) {
    vkDestroyPipeline(device, pipeline, nullptr);
    LOG_DEBUG("[Vulkan] Pipeline destroyed");
  }

  if (pipelineLayout) {
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    LOG_DEBUG("[Vulkan] Pipeline layout destroyed");
  }

  for (auto &x : descriptorSetLayouts) {
    vkDestroyDescriptorSetLayout(device, x, nullptr);
  }
  LOG_DEBUG("[Vulkan] Descriptor set layouts destroyed");
}

} // namespace liquid
