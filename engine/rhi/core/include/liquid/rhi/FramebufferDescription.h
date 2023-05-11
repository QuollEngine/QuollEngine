#pragma once

#include "RenderHandle.h"

namespace liquid::rhi {

/**
 * @brief Framebuffer description
 */
struct FramebufferDescription {
  /**
   * Render pass
   */
  RenderPassHandle renderPass = RenderPassHandle::Invalid;

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

} // namespace liquid::rhi
