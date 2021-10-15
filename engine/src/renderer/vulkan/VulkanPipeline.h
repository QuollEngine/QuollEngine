#pragma once

#include <vulkan/vulkan.hpp>

namespace liquid {

class VulkanPipeline {
public:
  /**
   * @brief Default constructor
   */
  VulkanPipeline() = default;

  /**
   * Constructor to set device and pipeline
   *
   * @param device Vulkan device
   * @param pipeline Vulkan pipeline handle
   */
  VulkanPipeline(VkDevice device, VkPipeline pipeline);

  /**
   * @brief Move constructor
   *
   * @param rhs Vulkan pipeline
   */
  VulkanPipeline(VulkanPipeline &&rhs);

  /**
   * @brief Move equality operator
   *
   * @param rhs Vulkan pipeline
   */
  VulkanPipeline &operator=(VulkanPipeline &&rhs);

  /**
   * @brief Copy constructor
   *
   * Deleted
   */
  VulkanPipeline &operator=(const VulkanPipeline &rhs) = delete;

  /**
   * @brief Copy equality operator
   *
   * Deleted
   */
  VulkanPipeline(const VulkanPipeline &rhs) = delete;

  /**
   * @brief Destructor
   *
   * Destroys Vulkan pipeline
   */
  ~VulkanPipeline();

  /**
   * @brief Get Vulkan pipeline
   *
   * @return Vulkan pipeline
   */
  inline VkPipeline getPipeline() const { return pipeline; }

private:
  VkDevice device = VK_NULL_HANDLE;
  VkPipeline pipeline = VK_NULL_HANDLE;
};

} // namespace liquid
