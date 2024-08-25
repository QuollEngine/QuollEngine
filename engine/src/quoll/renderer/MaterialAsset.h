#pragma once

#include "quoll/asset/AssetHandle.h"
#include "TextureAsset.h"

namespace quoll {

class Material;

struct MaterialAsset {
  AssetHandle<TextureAsset> baseColorTexture;

  i8 baseColorTextureCoord = -1;

  glm::vec4 baseColorFactor{};

  AssetHandle<TextureAsset> metallicRoughnessTexture;

  i8 metallicRoughnessTextureCoord = -1;

  f32 metallicFactor = 0.0f;

  f32 roughnessFactor = 0.0f;

  AssetHandle<TextureAsset> normalTexture;

  i8 normalTextureCoord = -1;

  f32 normalScale = 0.0f;

  AssetHandle<TextureAsset> occlusionTexture;

  i8 occlusionTextureCoord = -1;

  f32 occlusionStrength = 0.0f;

  AssetHandle<TextureAsset> emissiveTexture;

  i8 emissiveTextureCoord = -1;

  glm::vec3 emissiveFactor{};

  SharedPtr<Material> deviceHandle;
};

} // namespace quoll
