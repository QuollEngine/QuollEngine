#pragma once

#include "RenderHandle.h"
#include <vulkan/vulkan.hpp>

namespace liquid::rhi {

enum class AttachmentLoadOp { Load, Clear, DontCare };

enum class AttachmentStoreOp { Store, DontCare };

/**
 * @brief Depth stencil clear
 */
struct DepthStencilClear {
  /**
   * Depth clear value
   */
  float clearDepth = 0.0f;

  /**
   * Stencil clear value
   */
  uint32_t clearStencil = 0;
};

using AttachmentClearValue = std::variant<glm::vec4, DepthStencilClear>;

/**
 * @brief Render pass description
 */
struct RenderPassAttachmentDescription {
  /**
   * Attachment load operation
   */
  AttachmentLoadOp loadOp;

  /**
   * Attachment store operation
   */
  AttachmentStoreOp storeOp;

  /**
   * Attachment texture
   */
  TextureHandle texture = TextureHandle::Invalid;

  /**
   * Attachment image layout
   */
  VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;

  /**
   * Attachment clear value
   */
  AttachmentClearValue clearValue;
};

/**
 * @brief Render pass description
 */
struct RenderPassDescription {
  /**
   * Number of render pass attachments
   */
  std::vector<RenderPassAttachmentDescription> attachments;

  /**
   * Pipeline bind point
   */
  VkPipelineBindPoint bindPoint;
};

} // namespace liquid::rhi
