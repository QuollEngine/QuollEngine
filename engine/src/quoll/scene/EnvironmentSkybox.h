#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "quoll/asset/EnvironmentAsset.h"
#include "quoll/asset/Asset.h"

namespace quoll {

enum class EnvironmentSkyboxType { Color, Texture };

struct EnvironmentSkybox {
  EnvironmentSkyboxType type = EnvironmentSkyboxType::Color;

  EnvironmentAssetHandle texture = EnvironmentAssetHandle::Null;

  glm::vec4 color{0.0f, 0.0f, 0.0f, 1.0f};
};

} // namespace quoll
