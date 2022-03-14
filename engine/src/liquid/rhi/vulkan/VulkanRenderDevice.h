#pragma once

#include "liquid/rhi/base/RenderDevice.h"
#include "liquid/rhi/ResourceRegistry.h"

#include "liquid/renderer/render-graph/RenderGraph.h"
#include "liquid/renderer/render-graph/RenderGraphEvaluator.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDeviceObject.h"
#include "VulkanQueue.h"
#include "VulkanRenderContext.h"
#include "VulkanUploadContext.h"
#include "VulkanRenderBackend.h"
#include "VulkanResourceAllocator.h"
#include "VulkanResourceRegistry.h"
#include "VulkanCommandPool.h"
#include "VulkanDescriptorManager.h"
#include "VulkanSwapchain.h"

namespace liquid::rhi {

class VulkanRenderDevice : public RenderDevice {
public:
  /**
   * @brief Create Vulkan render device
   *
   * @param backend Render backend
   * @param physicalDevice Physical device
   * @param framebufferSize Framebuffer size
   */
  VulkanRenderDevice(VulkanRenderBackend &backend,
                     const VulkanPhysicalDevice &physicalDevice);

  /**
   * @brief Execute render graph
   *
   * @param graph Render graph
   * @param evaluator Render graph evaluator
   */
  void execute(RenderGraph &graph, RenderGraphEvaluator &evaluator);

  /**
   * @brief Wait for idle
   */
  void wait();

  /**
   * @brief Get Vulkan device handle
   *
   * @return Vulkan device handle
   */
  inline VulkanDeviceObject &getVulkanDevice() { return mDevice; }

  /**
   * @brief Get physical device
   *
   * @return Physical device
   */
  inline const VulkanPhysicalDevice &getPhysicalDevice() const {
    return mPhysicalDevice;
  }

private:
  /**
   * @brief Recreate swapchain
   */
  void recreateSwapchain();

  /**
   * @brief Synchronize swapchain images
   *
   * @param prevNumSwapchainImages Previous swapchain image count
   */
  void synchronizeSwapchain(size_t prevNumSwapchainImages);

  /**
   * @brief Synchronize resources
   *
   * @param registry Resource registry
   */
  void synchronize(ResourceRegistry &registry);

  /**
   * @brief Synchronize resource deletes
   *
   * @param registry Resource registry
   */
  void synchronizeDeletes(ResourceRegistry &registry);

private:
  VulkanRenderBackend &mBackend;
  VulkanPhysicalDevice mPhysicalDevice;
  VulkanDeviceObject mDevice;
  VulkanQueue mPresentQueue;
  VulkanQueue mGraphicsQueue;

  VulkanResourceAllocator mAllocator;
  VulkanResourceRegistry mRegistry;
  VulkanDescriptorManager mDescriptorManager;
  VulkanCommandPool mCommandPool;
  VulkanRenderContext mRenderContext;
  VulkanUploadContext mUploadContext;
  VulkanSwapchain mSwapchain;

  bool mSwapchainRecreated = false;
};

} // namespace liquid::rhi
