#pragma once

#include "Material.h"

namespace quoll {

/**
 * @brief PBR material instance
 */
class MaterialPBR : public Material {
public:
  /**
   * @brief PBR material properties
   */
  struct Properties {
  public:
    /**
     * @brief Get textures based on properties
     *
     * @return List of textures
     */
    const std::vector<rhi::TextureHandle> getTextures() const;

    /**
     * @brief Get Property objects
     *
     * @return List of properties
     */
    const std::vector<std::pair<String, Property>> getProperties() const;

  public:
    /**
     * Base color texture
     */
    rhi::TextureHandle baseColorTexture = rhi::TextureHandle::Null;

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
    rhi::TextureHandle metallicRoughnessTexture = rhi::TextureHandle::Null;

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
    rhi::TextureHandle normalTexture = rhi::TextureHandle::Null;

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
    rhi::TextureHandle occlusionTexture = rhi::TextureHandle::Null;

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
    rhi::TextureHandle emissiveTexture = rhi::TextureHandle::Null;

    /**
     * Emissive texture coordinate index
     */
    i8 emissiveTextureCoord = -1;

    /**
     * Emissive factor
     */
    glm::vec3 emissiveFactor{};
  };

public:
  /**
   * @brief Create PBR material
   *
   * @param name Material name
   * @param properties PBR properties
   * @param renderStorage Render storage
   */
  MaterialPBR(const String &name, const Properties &properties,
              RenderStorage &renderStorage);
};

} // namespace quoll
