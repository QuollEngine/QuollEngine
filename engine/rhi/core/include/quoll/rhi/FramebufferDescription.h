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
  uint32_t width = 0;

  /**
   * Height
   */
  uint32_t height = 0;

  /**
   * Number of layers
   */
  uint32_t layers = 0;

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
