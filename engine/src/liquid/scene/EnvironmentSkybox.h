#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/asset/EnvironmentAsset.h"

namespace liquid {

enum class EnvironmentSkyboxType { Color, Texture };

/**
 * @brief Environment skybox component
 */
struct EnvironmentSkybox {
  /**
   * Environment skybox type
   */
  EnvironmentSkyboxType type = EnvironmentSkyboxType::Color;

  /**
   * Skybox texture
   */
  EnvironmentAssetHandle texture = EnvironmentAssetHandle::Null;

  /**
   * Skybox color
   */
  glm::vec4 color{0.0f, 0.0f, 0.0f, 1.0f};
};

} // namespace liquid
