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
  int index = 0;
  int baseColorTextureIndex =
      rhi::isHandleValid(baseColorTexture) ? index++ : -1;
  int metallicRoughnessTextureIndex =
      rhi::isHandleValid(metallicRoughnessTexture) ? index++ : -1;
  int normalTextureIndex = rhi::isHandleValid(normalTexture) ? index++ : -1;
  int occlusionTextureIndex =
      rhi::isHandleValid(occlusionTexture) ? index++ : -1;
  int emissiveTextureIndex = rhi::isHandleValid(emissiveTexture) ? index++ : -1;

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
                         rhi::RenderDevice *device)
    : Material(properties.getTextures(), properties.getProperties(), device) {}

} // namespace liquid
