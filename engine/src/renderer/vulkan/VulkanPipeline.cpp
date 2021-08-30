#include "VulkanPipeline.h"
#include "core/EngineGlobals.h"

namespace liquid {

VulkanPipeline::VulkanPipeline(VkDevice device_, VkPipeline pipeline_)
    : device(device_), pipeline(pipeline_) {}

VulkanPipeline::VulkanPipeline(VulkanPipeline &&rhs) {
  pipeline = rhs.pipeline;
  device = rhs.device;
  rhs.pipeline = nullptr;
}

VulkanPipeline &VulkanPipeline::operator=(VulkanPipeline &&rhs) {
  pipeline = rhs.pipeline;
  device = rhs.device;
  rhs.pipeline = nullptr;
  return *this;
};

VulkanPipeline::~VulkanPipeline() {
  if (pipeline) {
    vkDestroyPipeline(device, pipeline, nullptr);
    LOG_DEBUG("[Vulkan] Pipeline destroyed");
  }
}

} // namespace liquid
