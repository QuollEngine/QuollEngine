#pragma once

#include "core/Base.h"
#include <vulkan/vulkan.hpp>

#include "renderer/Texture.h"
#include "VulkanHardwareBuffer.h"

namespace liquid {

class VulkanDescriptorManager {
public:
  /**
   * @brief Creates vulkan descriptor manager
   *
   * Creates descriptor pool and descriptor layouts
   */
  VulkanDescriptorManager(VkDevice device);

  /**
   * @brief Destroys descriptor manager
   *
   * Destroys descriptor pool and descriptor layouts
   */
  ~VulkanDescriptorManager();

  VulkanDescriptorManager(const VulkanDescriptorManager &rhs) = delete;
  VulkanDescriptorManager(VulkanDescriptorManager &&rhs) = delete;
  VulkanDescriptorManager &
  operator=(const VulkanDescriptorManager &rhs) = delete;
  VulkanDescriptorManager &operator=(VulkanDescriptorManager &&rhs) = delete;

  /**
   * @brief Create scene descriptor set
   *
   * @param cameraBuffer Camera buffer
   * @param sceneBuffer Scene buffer
   * @param shadowmaps Shadow maps
   * @param iblMaps Image based lighting maps
   * @return Scene descriptor set
   */
  VkDescriptorSet
  createSceneDescriptorSet(const SharedPtr<VulkanHardwareBuffer> &cameraBuffer,
                           const SharedPtr<VulkanHardwareBuffer> &sceneBuffer,
                           const SharedPtr<Texture> &shadowmaps,
                           const std::array<SharedPtr<Texture>, 3> &iblMaps);

  /**
   * @brief Create material descriptor set
   *
   * @param buffer Material buffer
   * @param textures Material textures
   *
   * @return Material descriptor set
   */
  VkDescriptorSet
  createMaterialDescriptorSet(const SharedPtr<VulkanHardwareBuffer> &buffer,
                              const std::vector<SharedPtr<Texture>> &textures);

  /**
   * @brief Get descriptor set layouts
   *
   * @return List of descriptor set layouts
   */
  inline std::array<VkDescriptorSetLayout, 2> getDescriptorLayouts() {
    return {sceneLayout, materialLayout};
  }

  /**
   * @brief Get descriptor pool
   *
   * @return Descriptor pool
   */
  inline VkDescriptorPool getDescriptorPool() const { return descriptorPool; }

  /**
   * @brief Get device
   *
   * @return Vulkan device
   */
  inline VkDevice getDevice() const { return device; }

private:
  /**
   * @brief Create scene descriptor layout
   */
  void createSceneDescriptorLayout();

  /**
   * @brief Create material descriptor layout
   */
  void createMaterialDescriptorLayout();

  /**
   * @brief Create descriptor pool
   */
  void createDescriptorPool();

public:
  VkDescriptorSetLayout sceneLayout = VK_NULL_HANDLE;
  VkDescriptorSetLayout materialLayout = VK_NULL_HANDLE;

  VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

  VkDevice device;
};

} // namespace liquid
