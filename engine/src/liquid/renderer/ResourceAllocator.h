#pragma once

#include "HardwareBuffer.h"
#include "Texture.h"

namespace liquid {

class ResourceAllocator {
public:
  /**
   * @brief Default destructor
   */
  virtual ~ResourceAllocator() = default;

  ResourceAllocator() = default;
  ResourceAllocator(const ResourceAllocator &rhs) = delete;
  ResourceAllocator(ResourceAllocator &&rhs) = delete;
  ResourceAllocator &operator=(const ResourceAllocator &rhs) = delete;
  ResourceAllocator &operator=(ResourceAllocator &&rhs) = delete;

  /**
   * @brief Create empty vertex buffer with size
   *
   * @param size Size
   * @return Vertex buffer
   */
  virtual SharedPtr<HardwareBuffer> createVertexBuffer(size_t size) = 0;

  /**
   * @brief Create empty index buffer with size
   *
   * @param size Size
   * @return Index buffer
   */
  virtual SharedPtr<HardwareBuffer> createIndexBuffer(size_t size) = 0;

  /**
   * @brief Create uniform buffer from arbitrary data
   *
   * @param size Buffer size
   * @return Uniform buffer
   */
  virtual SharedPtr<HardwareBuffer> createUniformBuffer(size_t size) = 0;

  /**
   * @brief Create 2D texture
   *
   * @param textureData Texture data
   * @return 2D texture
   */
  virtual SharedPtr<Texture> createTexture2D(const TextureData &data) = 0;

  /*
   * @brief Create cubemap texture
   *
   * @param textureData Texture data
   * @param Cubemap texture
   */
  virtual SharedPtr<Texture>
  createTextureCubemap(const TextureCubemapData &data) = 0;

  /**
   * @brief Create texture that is filled by framebuffer
   *
   * @param data Framebuffer texture data
   * @return Framebuffer texture
   */
  virtual SharedPtr<Texture>
  createTextureFramebuffer(const TextureFramebufferData &data) = 0;
};

} // namespace liquid
