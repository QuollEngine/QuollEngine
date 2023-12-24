#pragma once

#include "RenderHandle.h"

namespace quoll::rhi {

/**
 * @brief Framebuffer description
 */
struct FramebufferDescription {
  /**
   * Render pass
   */
  RenderPassHandle renderPass = RenderPassHandle::Null;

  /**
   * Width
   */
  u32 width = 0;

  /**
   * Height
   */
  u32 height = 0;

  /**
   * Number of layers
   */
  u32 layers = 0;

  /**
   * Texture attachments
   */
  std::vector<TextureHandle> attachments;

  /**
   * Debug name
   */
  String debugName;
};

} // namespace quoll::rhi
