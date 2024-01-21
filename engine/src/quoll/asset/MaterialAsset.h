#pragma once

namespace quoll {

class Material;

struct MaterialAsset {
  TextureAssetHandle baseColorTexture = TextureAssetHandle::Null;

  i8 baseColorTextureCoord = -1;

  glm::vec4 baseColorFactor{};

  TextureAssetHandle metallicRoughnessTexture = TextureAssetHandle::Null;

  i8 metallicRoughnessTextureCoord = -1;

  f32 metallicFactor = 0.0f;

  f32 roughnessFactor = 0.0f;

  TextureAssetHandle normalTexture = TextureAssetHandle::Null;

  i8 normalTextureCoord = -1;

  f32 normalScale = 0.0f;

  TextureAssetHandle occlusionTexture = TextureAssetHandle::Null;

  i8 occlusionTextureCoord = -1;

  f32 occlusionStrength = 0.0f;

  TextureAssetHandle emissiveTexture = TextureAssetHandle::Null;

  i8 emissiveTextureCoord = -1;

  glm::vec3 emissiveFactor{};

  SharedPtr<Material> deviceHandle;
};

} // namespace quoll
