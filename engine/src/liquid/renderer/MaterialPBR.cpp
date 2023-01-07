#include "liquid/core/Base.h"
#include "MaterialPBR.h"

namespace liquid {

const std::vector<rhi::TextureHandle>
MaterialPBR::Properties::getTextures() const {
  std::vector<rhi::TextureHandle> textures;
  if (rhi::isHandleValid(baseColorTexture)) {
    textures.push_back(baseColorTexture);
  }

  if (rhi::isHandleValid(metallicRoughnessTexture)) {
    textures.push_back(metallicRoughnessTexture);
  }

  if (rhi::isHandleValid(normalTexture)) {
    textures.push_back(normalTexture);
  }

  if (rhi::isHandleValid(occlusionTexture)) {
    textures.push_back(occlusionTexture);
  }

  if (rhi::isHandleValid(emissiveTexture)) {
    textures.push_back(emissiveTexture);
  }

  return textures;
}

const std::vector<std::pair<String, Property>>
MaterialPBR::Properties::getProperties() const {
  return {{"baseColorTexture", rhi::castHandleToUint(baseColorTexture)},
          {"baseColorTextureCoord", baseColorTextureCoord},
          {"baseColorFactor", baseColorFactor},
          {"metallicRoughnessTexture",
           rhi::castHandleToUint(metallicRoughnessTexture)},
          {"metallicRoughnessTextureCoord", metallicRoughnessTextureCoord},
          {"metallicFactor", metallicFactor},
          {"roughnessFactor", roughnessFactor},
          {"normalTexture", rhi::castHandleToUint(normalTexture)},
          {"normalTextureCoord", normalTextureCoord},
          {"normalScale", normalScale},
          {"occlusionTexture", rhi::castHandleToUint(occlusionTexture)},
          {"occlusionTextureCoord", occlusionTextureCoord},
          {"occlusionStrength", occlusionStrength},
          {"emissiveTexture", rhi::castHandleToUint(emissiveTexture)},
          {"emissiveTextureCoord", emissiveTextureCoord},
          {"emissiveFactor", emissiveFactor}};
}

MaterialPBR::MaterialPBR(const Properties &properties,
                         RenderStorage &renderStorage)
    : Material(properties.getTextures(), properties.getProperties(),
               renderStorage) {}

} // namespace liquid
