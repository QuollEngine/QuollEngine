#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/ImageLayout.h"
#include "quoll/rhi/PipelineBindPoint.h"

namespace quoll::rhi {

enum class AttachmentLoadOp { Load, Clear, DontCare };

enum class AttachmentStoreOp { Store, DontCare };

struct DepthStencilClear {
  f32 clearDepth = 0.0f;

  u32 clearStencil = 0;
};

using AttachmentClearValue = std::variant<glm::vec4, DepthStencilClear>;

struct RenderPassAttachmentDescription {
  AttachmentLoadOp loadOp;

  AttachmentStoreOp storeOp;

  AttachmentLoadOp stencilLoadOp;

  AttachmentStoreOp stencilStoreOp;

  TextureHandle texture = TextureHandle::Null;

  ImageLayout initialLayout{ImageLayout::Undefined};

  ImageLayout layout{ImageLayout::Undefined};

  AttachmentClearValue clearValue;
};

struct RenderPassDescription {
  std::vector<RenderPassAttachmentDescription> colorAttachments;

  std::optional<RenderPassAttachmentDescription> depthAttachment;

  std::optional<RenderPassAttachmentDescription> resolveAttachment;

  PipelineBindPoint bindPoint;

  String debugName;
};

} // namespace quoll::rhi
