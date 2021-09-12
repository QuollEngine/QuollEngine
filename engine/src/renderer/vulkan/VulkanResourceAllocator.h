#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "renderer/ResourceAllocator.h"
#include "profiler/StatsManager.h"

#include "VulkanHardwareBuffer.h"
#include "VulkanUploadContext.h"

namespace liquid {

class VulkanResourceAllocator : public ResourceAllocator {
public:
  /**
   * @brief Create Vulkan resource allocator
   *
   * @param vulkanInstance Vulkan instance
   * @param uploadContext Vulkan upload context
   * @param statsManager Stats manager
   */
  static VulkanResourceAllocator *
  create(const VulkanContext &vulkanInstance,
         const VulkanUploadContext &uploadContext, StatsManager &statsManager);

public:
  /**
   * @brief Constructor
   *
   * Sets allocator for creating buffers
   *
   * @param uploadContext Upload context
   * @param allocator Vma Allocator
   * @param device Vulkan device
   * @param statsManager Stats manager
   */
  VulkanResourceAllocator(const VulkanUploadContext &uploadContext,
                          VmaAllocator allocator, VkDevice device,
                          StatsManager &statsManager);

  /**
   * @brief Destroys Vulkan resource allocator
   */
  ~VulkanResourceAllocator();

  VulkanResourceAllocator(const VulkanResourceAllocator &rhs) = delete;
  VulkanResourceAllocator(VulkanResourceAllocator &&rhs) = delete;
  VulkanResourceAllocator &
  operator=(const VulkanResourceAllocator &rhs) = delete;
  VulkanResourceAllocator &operator=(VulkanResourceAllocator &&rhs) = delete;

  /**
   * @brief Create empty vertex buffer with size
   *
   * @param size Size
   * @return Vertex buffer
   */
  SharedPtr<HardwareBuffer> createVertexBuffer(size_t size) override;

  /**
   * @brief Create empty index buffer with size
   *
   * @param size Size
   * @return Index buffer
   */
  SharedPtr<HardwareBuffer> createIndexBuffer(size_t size) override;

  /**
   * @brief Create uniform buffer from arbitrary data
   *
   * @param size Buffer size
   * @return Uniform buffer
   */
  SharedPtr<HardwareBuffer> createUniformBuffer(size_t size) override;

  /**
   * @brief Create 2D texture from incoming data
   *
   * @param data Texture data
   * @return 2D texture
   */
  SharedPtr<Texture> createTexture2D(const TextureData &data) override;

  /**
   * @brief Create cubemap texture from incoming data
   *
   * @param data Texture data
   * @return Cubemap texture
   */
  SharedPtr<Texture>
  createTextureCubemap(const TextureCubemapData &data) override;

  /**
   * @brief Create texture that is filled by framebuffer
   *
   * @param data Framebuffer texture data
   * @return Framebuffer texture
   */
  SharedPtr<Texture>
  createTextureFramebuffer(const TextureFramebufferData &data) override;

  /**
   * @brief Get Vma allocator
   *
   * @return Vma allocator
   */
  inline VmaAllocator getVmaAllocator() { return allocator; }

private:
  SharedPtr<VulkanHardwareBuffer>
  createHardwareBuffer(HardwareBuffer::HardwareBufferType bufferType,
                       size_t bufferSize);

private:
  const VulkanUploadContext &uploadContext;
  VmaAllocator allocator = nullptr;
  VkDevice device;
  StatsManager &statsManager;
};

} // namespace liquid
