#pragma once

namespace liquid {

struct MaterialAsset {
  TextureAssetHandle baseColorTexture = TextureAssetHandle::Invalid;
  int baseColorTextureCoord = -1;
  glm::vec4 baseColorFactor{};

  TextureAssetHandle metallicRoughnessTexture = TextureAssetHandle::Invalid;
  int metallicRoughnessTextureCoord = -1;
  float metallicFactor = 0.0f;
  float roughnessFactor = 0.0f;

  TextureAssetHandle normalTexture = TextureAssetHandle::Invalid;
  int normalTextureCoord = -1;
  float normalScale = 0.0f;

  TextureAssetHandle occlusionTexture = TextureAssetHandle::Invalid;
  int occlusionTextureCoord = -1;
  float occlusionStrength = 0.0f;

  TextureAssetHandle emissiveTexture = TextureAssetHandle::Invalid;
  int emissiveTextureCoord = -1;
  glm::vec3 emissiveFactor{};
};

} // namespace liquid
