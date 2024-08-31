#pragma once

#include "quoll/asset/AssetRef.h"
#include "TextureAsset.h"

namespace quoll {

class Material;

struct MaterialAsset {
  AssetRef<TextureAsset> baseColorTexture;

  i8 baseColorTextureCoord = -1;

  glm::vec4 baseColorFactor{};

  AssetRef<TextureAsset> metallicRoughnessTexture;

  i8 metallicRoughnessTextureCoord = -1;

  f32 metallicFactor = 0.0f;

  f32 roughnessFactor = 0.0f;

  AssetRef<TextureAsset> normalTexture;

  i8 normalTextureCoord = -1;

  f32 normalScale = 0.0f;

  AssetRef<TextureAsset> occlusionTexture;

  i8 occlusionTextureCoord = -1;

  f32 occlusionStrength = 0.0f;

  AssetRef<TextureAsset> emissiveTexture;

  i8 emissiveTextureCoord = -1;

  glm::vec3 emissiveFactor{};

  SharedPtr<Material> deviceHandle;
};

} // namespace quoll
