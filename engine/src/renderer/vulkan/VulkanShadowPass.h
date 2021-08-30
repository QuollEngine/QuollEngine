#pragma once

#include <vulkan/vulkan.hpp>
#include "VulkanContext.h"
#include "VulkanPipelineBuilder.h"
#include "VulkanDescriptorManager.h"
#include "VulkanResourceManager.h"
#include "renderer/ResourceAllocator.h"
#include "renderer/Material.h"

namespace liquid {

class VulkanShadowPass {
public:
  /**
   * @brief Create shadow pass
   *
   * @param shadowmapDimensions Shadowmap dimensions
   * @param device Vulkan device
   * @param allocator Vma Allocator
   * @param pipelineBuilder Vma Allocator
   * @param resourceAllocator Resource allocator
   * @param descriptorManager Descriptor manager
   * @param statsManager Stats manager
   */
  VulkanShadowPass(uint32_t shadowmapDimensions, VkDevice device,
                   VmaAllocator allocator,
                   VulkanPipelineBuilder *pipelineBuilder,
                   ResourceAllocator *resourceAllocator,
                   VulkanDescriptorManager *descriptorManager,
                   const SharedPtr<StatsManager> &statsManager);

  /**
   * @brief Destroy all resources related to this pass
   */
  ~VulkanShadowPass();

  VulkanShadowPass(const VulkanShadowPass &rhs) = delete;
  VulkanShadowPass(VulkanShadowPass &&rhs) = delete;
  VulkanShadowPass &operator=(const VulkanShadowPass &rhs) = delete;
  VulkanShadowPass &operator=(VulkanShadowPass &&rhs) = delete;

  /**
   * @brief Process commands in render pass
   *
   * @param commandBuffer Command buffer
   * @param renderFn Render function to process
   */
  void render(VkCommandBuffer buffer,
              std::function<void(VkCommandBuffer cmd)> renderFn);

  /**
   * @brief Get resource manager
   *
   * @return Resource manager
   */
  inline const SharedPtr<VulkanResourceManager> &getResourceManager() {
    return resourceManager;
  }

  /**
   * @brief Get shadow map
   *
   * @return Shadow map texture
   */
  inline const SharedPtr<Texture> &getShadowmap() { return shadowmapTexture; }

  /**
   * @brief Get shadowmap extent
   *
   * @return Shadowmap Extent
   */
  inline const VkExtent2D &getExtent() const { return shadowmapExtent; }

private:
  /**
   * @brief Create render pass
   */
  void createRenderPass();

  /**
   * @brief Create resource manager
   *
   * @param pipelineBuilder Pipeline builder
   * @param descriptorManager Descriptor manager
   */
  void createResourceManager(VulkanPipelineBuilder *pipelineBuilder,
                             VulkanDescriptorManager *descriptorManager);

  /**
   * @brief Create shadow map texture
   *
   * @param allocator Vma Allocator
   * @param statsManager Stats manager
   */
  void createTextures(VmaAllocator allocator,
                      const SharedPtr<StatsManager> &statsManager);

  /**
   * @brief Create framebuffer
   */
  void createFramebuffers();

private:
  VkExtent2D shadowmapExtent{};
  VkDevice device = nullptr;
  VkRenderPass renderPass = nullptr;
  SharedPtr<VulkanResourceManager> resourceManager = nullptr;
  ResourceAllocator *resourceAllocator = nullptr;
  SharedPtr<Texture> shadowmapTexture;
  VkFramebuffer framebuffer = nullptr;
};

} // namespace liquid
