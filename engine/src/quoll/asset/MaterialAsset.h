#pragma once

#include "quoll/renderer/Material.h"

namespace quoll {

/**
 * @brief Material asset data
 */
struct MaterialAsset {
  /**
   * Base color texture
   */
  TextureAssetHandle baseColorTexture = TextureAssetHandle::Null;

  /**
   * Base color texture coordinate index
   */
  i8 baseColorTextureCoord = -1;

  /**
   * Base color factor
   */
  glm::vec4 baseColorFactor{};

  /**
   * Metallic roughness textures
   */
  TextureAssetHandle metallicRoughnessTexture = TextureAssetHandle::Null;

  /**
   * Metallic roughness texture coordinate index
   */
  i8 metallicRoughnessTextureCoord = -1;

  /**
   * Metallic factor
   */
  f32 metallicFactor = 0.0f;

  /**
   * Roughness factor
   */
  f32 roughnessFactor = 0.0f;

  /**
   * Normal texture
   */
  TextureAssetHandle normalTexture = TextureAssetHandle::Null;

  /**
   * Normal texture coordinate index
   */
  i8 normalTextureCoord = -1;

  /**
   * Normal scale
   */
  f32 normalScale = 0.0f;

  /**
   * Occlusion texture
   */
  TextureAssetHandle occlusionTexture = TextureAssetHandle::Null;

  /**
   * Occlusion texture coordinate index
   */
  i8 occlusionTextureCoord = -1;

  /**
   * Occlusion strength
   */
  f32 occlusionStrength = 0.0f;

  /**
   * Emissive texture
   */
  TextureAssetHandle emissiveTexture = TextureAssetHandle::Null;

  /**
   * Emissive texture coordinate index
   */
  i8 emissiveTextureCoord = -1;

  /**
   * Emissive factor
   */
  glm::vec3 emissiveFactor{};

  /**
   * Material device handle
   */
  SharedPtr<Material> deviceHandle;
};

} // namespace quoll
