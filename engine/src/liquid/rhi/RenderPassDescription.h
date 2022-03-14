#pragma once

#include "RenderHandle.h"
#include "liquid/renderer/render-graph/RenderGraphAttachmentDescription.h"
#include <vulkan/vulkan.hpp>

namespace liquid::rhi {

struct RenderPassAttachmentDescription {
  AttachmentLoadOp loadOp;
  AttachmentStoreOp storeOp;
  TextureHandle texture = 0;
  VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;
};

struct RenderPassDescription {
  std::vector<RenderPassAttachmentDescription> colorAttachments;
  RenderPassAttachmentDescription depthAttachment;
  VkPipelineBindPoint bindPoint;
};

} // namespace liquid::rhi
