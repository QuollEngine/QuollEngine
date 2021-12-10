#include "core/Base.h"
#include "core/EngineGlobals.h"
#include "VulkanPipeline.h"

namespace liquid {

VulkanPipeline::VulkanPipeline(VkDevice device_, VkPipeline pipeline_)
    : device(device_), pipeline(pipeline_) {}

VulkanPipeline::~VulkanPipeline() {
  if (pipeline) {
    vkDestroyPipeline(device, pipeline, nullptr);
    LOG_DEBUG("[Vulkan] Pipeline destroyed");
  }
}

} // namespace liquid
