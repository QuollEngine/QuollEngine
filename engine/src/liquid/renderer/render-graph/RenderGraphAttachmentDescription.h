#pragma once

namespace liquid {

enum class AttachmentLoadOp { Load, Clear, DontCare };

enum class AttachmentStoreOp { Store, DontCare };

struct DepthStencilClear {
  float clearDepth = 0.0f;
  uint32_t clearStencil = 0;
};

struct RenderPassAttachment {
  AttachmentLoadOp loadOp;
  AttachmentStoreOp storeOp;
  std::variant<glm::vec4, DepthStencilClear> clearValue;
};

} // namespace liquid
