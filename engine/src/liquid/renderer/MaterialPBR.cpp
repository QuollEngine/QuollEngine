#include "liquid/core/Base.h"
#include "MaterialPBR.h"

namespace liquid {

const std::vector<SharedPtr<Texture>>
MaterialPBR::Properties::getTextures() const {
  std::vector<SharedPtr<Texture>> textures;
  if (baseColorTexture) {
    textures.push_back(baseColorTexture);
  }

  if (metallicRoughnessTexture) {
    textures.push_back(metallicRoughnessTexture);
  }

  if (normalTexture) {
    textures.push_back(normalTexture);
  }

  if (occlusionTexture) {
    textures.push_back(occlusionTexture);
  }

  if (emissiveTexture) {
    textures.push_back(emissiveTexture);
  }

  return textures;
}

const std::vector<std::pair<String, Property>>
MaterialPBR::Properties::getProperties() const {
  int index = 0;
  int baseColorTextureIndex = baseColorTexture ? index++ : -1;
  int metallicRoughnessTextureIndex = metallicRoughnessTexture ? index++ : -1;
  int normalTextureIndex = normalTexture ? index++ : -1;
  int occlusionTextureIndex = occlusionTexture ? index++ : -1;
  int emissiveTextureIndex = emissiveTexture ? index++ : -1;

  return {{"baseColorTexture", baseColorTextureIndex},
          {"baseColorTextureCoord", baseColorTextureCoord},
          {"baseColorFactor", baseColorFactor},
          {"metallicRoughnessTexture", metallicRoughnessTextureIndex},
          {"metallicRoughnessTextureCoord", metallicRoughnessTextureCoord},
          {"metallicFactor", metallicFactor},
          {"roughnessFactor", roughnessFactor},
          {"normalTexture", normalTextureIndex},
          {"normalTextureCoord", normalTextureCoord},
          {"normalScale", normalScale},
          {"occlusionTexture", occlusionTextureIndex},
          {"occlusionTextureCoord", occlusionTextureCoord},
          {"occlusionStrength", occlusionStrength},
          {"emissiveTexture", emissiveTextureIndex},
          {"emissiveTextureCoord", emissiveTextureCoord},
          {"emissiveFactor", emissiveFactor}};
}

MaterialPBR::MaterialPBR(const Properties &properties,
                         ResourceAllocator *resourceAllocator)
    : Material(properties.getTextures(), properties.getProperties(),
               resourceAllocator) {}
} // namespace liquid
