#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/ImageLayout.h"
#include "liquid/rhi/PipelineBindPoint.h"

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
   * Attachment stencil load operation
   */
  AttachmentLoadOp stencilLoadOp;

  /**
   * Attachment stencil store operation
   */
  AttachmentStoreOp stencilStoreOp;

  /**
   * Attachment texture
   */
  TextureHandle texture = TextureHandle::Invalid;

  /**
   * Attachment image initial layout
   */
  ImageLayout initialLayout{ImageLayout::Undefined};

  /**
   * Attachment image layout
   */
  ImageLayout layout{ImageLayout::Undefined};

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
   * Color attachments
   */
  std::vector<RenderPassAttachmentDescription> colorAttachments;

  /**
   * Depth attachment
   */
  std::optional<RenderPassAttachmentDescription> depthAttachment;

  /**
   * Resolve attachment
   */
  std::optional<RenderPassAttachmentDescription> resolveAttachment;

  /**
   * Pipeline bind point
   */
  PipelineBindPoint bindPoint;
};

} // namespace liquid::rhi
