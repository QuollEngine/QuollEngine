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
  int baseColorTextureIndex = rhi::isHandleValid(baseColorTexture)
                                  ? static_cast<int32_t>(baseColorTexture)
                                  : -1;
  int metallicRoughnessTextureIndex =
      rhi::isHandleValid(metallicRoughnessTexture)
          ? static_cast<int32_t>(metallicRoughnessTexture)
          : -1;
  int normalTextureIndex = rhi::isHandleValid(normalTexture)
                               ? static_cast<int32_t>(normalTexture)
                               : -1;
  int occlusionTextureIndex = rhi::isHandleValid(occlusionTexture)
                                  ? static_cast<int32_t>(occlusionTexture)
                                  : -1;
  int emissiveTextureIndex = rhi::isHandleValid(emissiveTexture)
                                 ? static_cast<int32_t>(emissiveTexture)
                                 : -1;

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
