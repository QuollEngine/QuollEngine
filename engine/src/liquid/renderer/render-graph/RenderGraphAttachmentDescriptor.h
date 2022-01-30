#pragma once

namespace liquid {

enum class AttachmentLoadOp { Load, Clear, DontCare };

enum class AttachmentStoreOp { Store, DontCare };

enum class AttachmentType { Color, Depth };

enum class AttachmentSizeMethod { Fixed, SwapchainRelative };

struct DepthStencilClear {
  float clearDepth = 0.0f;
  uint32_t clearStencil = 0;
};

struct AttachmentData {
  AttachmentType type = AttachmentType::Color;
  AttachmentSizeMethod sizeMethod = AttachmentSizeMethod::Fixed;
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;
  uint32_t format = 0;
  std::variant<glm::vec4, DepthStencilClear> clearValue;
};

struct RenderPassAttachment {
  AttachmentLoadOp loadOp;
  AttachmentStoreOp storeOp;
  std::variant<glm::vec4, DepthStencilClear> clearValue;
};

} // namespace liquid
