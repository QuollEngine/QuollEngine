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
    const std::vector<SharedPtr<Texture>> getTextures() const;

    /**
     * @brief Get Property objects
     *
     * @return List of properties
     */
    const std::vector<std::pair<String, Property>> getProperties() const;

  public:
    SharedPtr<Texture> baseColorTexture;
    int baseColorTextureCoord = -1;
    glm::vec4 baseColorFactor;

    SharedPtr<Texture> metallicRoughnessTexture;
    int metallicRoughnessTextureCoord = -1;
    float metallicFactor = 0.0f;
    float roughnessFactor = 0.0f;

    SharedPtr<Texture> normalTexture;
    int normalTextureCoord = -1;
    float normalScale = 0.0f;

    SharedPtr<Texture> occlusionTexture;
    int occlusionTextureCoord = -1;
    float occlusionStrength = 0.0f;

    SharedPtr<Texture> emissiveTexture;
    int emissiveTextureCoord = -1;
    glm::vec3 emissiveFactor;
  };

public:
  /**
   * @brief Create PBR material
   *
   * @param properties PBR properties
   * @param resourceAllocator Resource allocator
   */
  MaterialPBR(const Properties &properties,
              ResourceAllocator *resourceAllocator);
};

} // namespace liquid
