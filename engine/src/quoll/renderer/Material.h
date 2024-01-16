#pragma once

#include "quoll/core/Property.h"

#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/Descriptor.h"
#include "quoll/renderer/RenderStorage.h"

namespace quoll {

class Material {
public:
  Material(const String &name, const std::vector<rhi::TextureHandle> &textures,
           const std::vector<std::pair<String, Property>> &properties,
           RenderStorage &renderStorage);

  void updateProperty(StringView name, const Property &value);

  inline const std::vector<rhi::TextureHandle> &getTextures() const {
    return mTextures;
  }

  inline bool hasTextures() const { return !mTextures.empty(); }

  inline rhi::BufferHandle getBuffer() const { return mBuffer.getHandle(); }

  inline rhi::DeviceAddress getAddress() const { return mBuffer.getAddress(); }

  inline const std::vector<Property> &getProperties() const {
    return mProperties;
  }

private:
  usize updateBufferData();

private:
  std::vector<rhi::TextureHandle> mTextures;
  rhi::Buffer mBuffer;

  char *mData = nullptr;

  std::vector<Property> mProperties;
  std::map<String, usize> mPropertyMap;
};

} // namespace quoll
