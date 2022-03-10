#include "liquid/core/Base.h"
#include "MaterialPBR.h"

namespace liquid {

const std::vector<TextureHandle> MaterialPBR::Properties::getTextures() const {
  std::vector<TextureHandle> textures;
  if (baseColorTexture > 0) {
    textures.push_back(baseColorTexture);
  }

  if (metallicRoughnessTexture > 0) {
    textures.push_back(metallicRoughnessTexture);
  }

  if (normalTexture > 0) {
    textures.push_back(normalTexture);
  }

  if (occlusionTexture > 0) {
    textures.push_back(occlusionTexture);
  }

  if (emissiveTexture > 0) {
    textures.push_back(emissiveTexture);
  }

  return textures;
}

const std::vector<std::pair<String, Property>>
MaterialPBR::Properties::getProperties() const {
  int index = 0;
  int baseColorTextureIndex = baseColorTexture > 0 ? index++ : -1;
  int metallicRoughnessTextureIndex =
      metallicRoughnessTexture > 0 ? index++ : -1;
  int normalTextureIndex = normalTexture > 0 ? index++ : -1;
  int occlusionTextureIndex = occlusionTexture > 0 ? index++ : -1;
  int emissiveTextureIndex = emissiveTexture > 0 ? index++ : -1;

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
                         experimental::ResourceRegistry &registry)
    : Material(properties.getTextures(), properties.getProperties(), registry) {
}
} // namespace liquid
