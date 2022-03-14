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
    rhi::TextureHandle baseColorTexture = 0;
    int baseColorTextureCoord = -1;
    glm::vec4 baseColorFactor;

    rhi::TextureHandle metallicRoughnessTexture = 0;
    int metallicRoughnessTextureCoord = -1;
    float metallicFactor = 0.0f;
    float roughnessFactor = 0.0f;

    rhi::TextureHandle normalTexture = 0;
    int normalTextureCoord = -1;
    float normalScale = 0.0f;

    rhi::TextureHandle occlusionTexture = 0;
    int occlusionTextureCoord = -1;
    float occlusionStrength = 0.0f;

    rhi::TextureHandle emissiveTexture = 0;
    int emissiveTextureCoord = -1;
    glm::vec3 emissiveFactor;
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
