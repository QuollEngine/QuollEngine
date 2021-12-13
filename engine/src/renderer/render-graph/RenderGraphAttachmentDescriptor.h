#pragma once

#include "renderer/Texture.h"

namespace liquid {

enum class AttachmentLoadOp { Load, Clear, DontCare };

enum class AttachmentStoreOp { Store, DontCare };

enum class AttachmentType { Color, Depth };

struct DepthStencilClear {
  float clearDepth = 0.0f;
  uint32_t clearStencil = 0;
};

struct RenderPassAttachment {
  AttachmentType type;
  TextureFramebufferData textureData{};
  AttachmentLoadOp loadOp;
  AttachmentStoreOp storeOp;
  std::variant<glm::vec4, DepthStencilClear> clearValue;
};

struct RenderPassSwapchainAttachment {
  AttachmentType type;
  AttachmentLoadOp loadOp;
  AttachmentStoreOp storeOp;
  std::variant<glm::vec4, DepthStencilClear> clearValue;
};

} // namespace liquid
