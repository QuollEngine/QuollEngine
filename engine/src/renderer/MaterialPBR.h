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
    int baseColorTextureCoord;
    glm::vec4 baseColorFactor;

    SharedPtr<Texture> metallicRoughnessTexture;
    int metallicRoughnessTextureCoord;
    float metallicFactor;
    float roughnessFactor;

    SharedPtr<Texture> normalTexture;
    int normalTextureCoord;
    float normalScale;

    SharedPtr<Texture> occlusionTexture;
    int occlusionTextureCoord;
    float occlusionStrength;

    SharedPtr<Texture> emissiveTexture;
    int emissiveTextureCoord;
    glm::vec3 emissiveFactor;
  };

public:
  /**
   * @brief Create PBR material
   *
   * @param properties PBR properties
   * @param resourceAllocator Resource allocator
   * @param resourceManager Resource manager
   */
  MaterialPBR(const Properties &properties,
              ResourceAllocator *resourceAllocator,
              ResourceManager *resourceManager);
};

} // namespace liquid
