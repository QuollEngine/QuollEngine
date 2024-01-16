#pragma once

#include "Material.h"

namespace quoll {

class MaterialPBR : public Material {
public:
  struct Properties {
  public:
    const std::vector<rhi::TextureHandle> getTextures() const;

    const std::vector<std::pair<String, Property>> getProperties() const;

  public:
    rhi::TextureHandle baseColorTexture = rhi::TextureHandle::Null;

    i8 baseColorTextureCoord = -1;

    glm::vec4 baseColorFactor{};

    rhi::TextureHandle metallicRoughnessTexture = rhi::TextureHandle::Null;

    i8 metallicRoughnessTextureCoord = -1;

    f32 metallicFactor = 0.0f;

    f32 roughnessFactor = 0.0f;

    rhi::TextureHandle normalTexture = rhi::TextureHandle::Null;

    i8 normalTextureCoord = -1;

    f32 normalScale = 0.0f;

    rhi::TextureHandle occlusionTexture = rhi::TextureHandle::Null;

    i8 occlusionTextureCoord = -1;

    f32 occlusionStrength = 0.0f;

    rhi::TextureHandle emissiveTexture = rhi::TextureHandle::Null;

    i8 emissiveTextureCoord = -1;

    glm::vec3 emissiveFactor{};
  };

public:
  MaterialPBR(const String &name, const Properties &properties,
              RenderStorage &renderStorage);
};

} // namespace quoll
