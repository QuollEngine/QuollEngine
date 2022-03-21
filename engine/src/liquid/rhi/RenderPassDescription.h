#pragma once

#include "RenderHandle.h"
#include <vulkan/vulkan.hpp>

namespace liquid::rhi {

enum class AttachmentLoadOp { Load, Clear, DontCare };

enum class AttachmentStoreOp { Store, DontCare };

struct DepthStencilClear {
  float clearDepth = 0.0f;
  uint32_t clearStencil = 0;
};

using AttachmentClearValue = std::variant<glm::vec4, DepthStencilClear>;

struct RenderPassAttachmentDescription {
  AttachmentLoadOp loadOp;
  AttachmentStoreOp storeOp;
  TextureHandle texture = TextureHandle::Invalid;
  VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;
  AttachmentClearValue clearValue;
};

struct RenderPassDescription {
  std::vector<RenderPassAttachmentDescription> attachments;
  VkPipelineBindPoint bindPoint;
};

} // namespace liquid::rhi
