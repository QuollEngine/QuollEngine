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
  virtual HardwareBuffer *createVertexBuffer(size_t size) = 0;

  /**
   * @brief Creates vertex buffer from vertices
   *
   * @param vertices List of vertices
   * @return Vertex buffer
   */
  virtual HardwareBuffer *
  createVertexBuffer(const std::vector<Vertex> &vertices) = 0;

  /**
   * @brief Create empty index buffer with size
   *
   * @param size Size
   * @return Index buffer
   */
  virtual HardwareBuffer *createIndexBuffer(size_t size) = 0;

  /**
   * @brief Creates index buffer from indices
   *
   * @param indices List of indices
   * @return Index buffer
   */
  virtual HardwareBuffer *
  createIndexBuffer(const std::vector<uint32_t> &indices) = 0;

  /**
   * @brief Create uniform buffer from arbitrary data
   *
   * @param bufferSize Buffer size
   * @return Uniform buffer
   */
  virtual HardwareBuffer *createUniformBuffer(size_t bufferSize) = 0;

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
   * @brief Create shadowmap texture
   *
   * @param dimensions Dimensions
   * @param layers Number of layers
   * @return Shadowmap texture
   */
  virtual SharedPtr<Texture> createTextureShadowmap(uint32_t dimensions,
                                                    uint32_t layers) = 0;
};

} // namespace liquid
