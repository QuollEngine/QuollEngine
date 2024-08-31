#pragma once

#include "quoll/asset/AssetHandle.h"
#include "quoll/rhi/RenderHandle.h"
#include "EnvironmentAsset.h"

namespace quoll {

enum class EnvironmentSkyboxType { Color, Texture };

struct EnvironmentSkybox {
  EnvironmentSkyboxType type = EnvironmentSkyboxType::Color;

  AssetRef<EnvironmentAsset> texture;

  glm::vec4 color{0.0f, 0.0f, 0.0f, 1.0f};
};

} // namespace quoll
