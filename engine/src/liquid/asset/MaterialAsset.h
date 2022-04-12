#pragma once

namespace liquid {

struct MaterialAsset {
  TextureAssetHandle baseColorTexture = TextureAssetHandle::Invalid;
  int8_t baseColorTextureCoord = -1;
  glm::vec4 baseColorFactor{};

  TextureAssetHandle metallicRoughnessTexture = TextureAssetHandle::Invalid;
  int8_t metallicRoughnessTextureCoord = -1;
  float metallicFactor = 0.0f;
  float roughnessFactor = 0.0f;

  TextureAssetHandle normalTexture = TextureAssetHandle::Invalid;
  int8_t normalTextureCoord = -1;
  float normalScale = 0.0f;

  TextureAssetHandle occlusionTexture = TextureAssetHandle::Invalid;
  int8_t occlusionTextureCoord = -1;
  float occlusionStrength = 0.0f;

  TextureAssetHandle emissiveTexture = TextureAssetHandle::Invalid;
  int8_t emissiveTextureCoord = -1;
  glm::vec3 emissiveFactor{};
};

} // namespace liquid
