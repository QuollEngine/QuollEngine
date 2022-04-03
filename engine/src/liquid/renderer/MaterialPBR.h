#pragma once

#include "Material.h"
#include "ShaderLibrary.h"

namespace liquid {

class MaterialPBR : public Material {
public:
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
    rhi::TextureHandle baseColorTexture = rhi::TextureHandle::Invalid;
    int baseColorTextureCoord = -1;
    glm::vec4 baseColorFactor{};

    rhi::TextureHandle metallicRoughnessTexture = rhi::TextureHandle::Invalid;
    int metallicRoughnessTextureCoord = -1;
    float metallicFactor = 0.0f;
    float roughnessFactor = 0.0f;

    rhi::TextureHandle normalTexture = rhi::TextureHandle::Invalid;
    int normalTextureCoord = -1;
    float normalScale = 0.0f;

    rhi::TextureHandle occlusionTexture = rhi::TextureHandle::Invalid;
    int occlusionTextureCoord = -1;
    float occlusionStrength = 0.0f;

    rhi::TextureHandle emissiveTexture = rhi::TextureHandle::Invalid;
    int emissiveTextureCoord = -1;
    glm::vec3 emissiveFactor{};
  };

public:
  /**
   * @brief Create PBR material
   *
   * @param properties PBR properties
   * @param registry Resource registry
   */
  MaterialPBR(const Properties &properties, rhi::ResourceRegistry &registry);
};

} // namespace liquid
