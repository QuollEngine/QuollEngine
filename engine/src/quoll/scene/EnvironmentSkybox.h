#pragma once

#include "quoll/asset/Asset.h"
#include "quoll/rhi/RenderHandle.h"

namespace quoll {

enum class EnvironmentSkyboxType { Color, Texture };

struct EnvironmentSkybox {
  EnvironmentSkyboxType type = EnvironmentSkyboxType::Color;

  EnvironmentAssetHandle texture = EnvironmentAssetHandle::Null;

  glm::vec4 color{0.0f, 0.0f, 0.0f, 1.0f};
};

} // namespace quoll
