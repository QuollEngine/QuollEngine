#pragma once

#include "quoll/core/Property.h"

#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/Descriptor.h"
#include "quoll/renderer/RenderStorage.h"

namespace quoll {

/**
 * @brief Material instance
 */
class Material {
public:
  /**
   * @brief Creates material
   *
   * @param name Material name
   * @param textures Textures
   * @param properties Material properties
   * @param renderStorage Render storage
   */
  Material(const String &name, const std::vector<rhi::TextureHandle> &textures,
           const std::vector<std::pair<String, Property>> &properties,
           RenderStorage &renderStorage);

  /**
   * @brief Update property
   *
   * @param name Property name
   * @param value Property value
   */
  void updateProperty(StringView name, const Property &value);

  /**
   * @brief Get texture handles
   *
   * @return List of texture handles
   */
  inline const std::vector<rhi::TextureHandle> &getTextures() const {
    return mTextures;
  }

  /**
   * @brief Check if there are any textures
   *
   * @retval true Has textures
   * @retval false Does not have textures
   */
  inline bool hasTextures() const { return !mTextures.empty(); }

  /**
   * @brief Gets uniform buffer
   *
   * @return Uniform buffer
   */
  inline rhi::BufferHandle getBuffer() const { return mBuffer.getHandle(); }

  /**
   * @brief Get buffer address
   *
   * @return Buffer device address
   */
  inline rhi::DeviceAddress getAddress() const { return mBuffer.getAddress(); }

  /**
   * @brief Get properties
   *
   * @return Unordered list of properties
   */
  inline const std::vector<Property> &getProperties() const {
    return mProperties;
  }

private:
  /**
   * @brief Update buffer data
   *
   * Merges all properties into a memory region
   *
   * @return Size of buffer data
   */
  size_t updateBufferData();

private:
  std::vector<rhi::TextureHandle> mTextures;
  rhi::Buffer mBuffer;

  char *mData = nullptr;

  std::vector<Property> mProperties;
  std::map<String, size_t> mPropertyMap;
};

} // namespace quoll
