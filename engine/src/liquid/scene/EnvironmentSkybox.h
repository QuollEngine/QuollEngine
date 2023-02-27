#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/asset/EnvironmentAsset.h"

namespace liquid {

/**
 * @brief Environment skybox component
 *
 * Stores IBL maps
 */
struct EnvironmentSkybox {

  /**
   * Environment asset handle
   */
  EnvironmentAssetHandle environmentHandle = EnvironmentAssetHandle::Invalid;

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
