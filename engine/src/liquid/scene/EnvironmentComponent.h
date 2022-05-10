#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid {

/**
 * @brief Environment component
 *
 * Stores IBL maps
 */
struct EnvironmentComponent {
  /**
   * Irradiance map texture
   */
  rhi::TextureHandle irradianceMap = rhi::TextureHandle::Invalid;

  /**
   * Specular map texture
   */
  rhi::TextureHandle specularMap = rhi::TextureHandle::Invalid;

  /**
   * BRDF LUT texture
   */
  rhi::TextureHandle brdfLUT = rhi::TextureHandle::Invalid;
};

} // namespace liquid
