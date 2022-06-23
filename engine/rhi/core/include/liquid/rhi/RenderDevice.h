#pragma once

#include "liquid/rhi/PhysicalDeviceInformation.h"

#include "DeviceStats.h"
#include "RenderGraph.h"
#include "RenderGraphEvaluator.h"
#include "Swapchain.h"
#include "RenderFrame.h"

namespace liquid::rhi {

/**
 * @brief Render device interface
 */
class RenderDevice {
public:
  RenderDevice(const RenderDevice &) = delete;
  RenderDevice &operator=(const RenderDevice &) = delete;
  RenderDevice(RenderDevice &&) = delete;
  RenderDevice &operator=(RenderDevice &&) = delete;

  /**
   * @brief Default constructor
   */
  RenderDevice() = default;

  /**
   * @brief Destroy render device
   */
  virtual ~RenderDevice() = default;

  /**
   * @brief Begin frame
   *
   * @return Frame data
   */
  virtual RenderFrame beginFrame() = 0;

  /**
   * @brief End frame
   *
   * @param renderFrame Render frame
   */
  virtual void endFrame(const RenderFrame &renderFrame) = 0;

  /**
   * @brief Wait for device to be idle
   */
  virtual void waitForIdle() = 0;

  /**
   * @brief Synchronize resources
   *
   * @param registry Resource registry
   */
  virtual void synchronize(ResourceRegistry &registry) = 0;

  /**
   * @brief Get physical device information
   *
   * @return Physical device information
   */
  virtual const PhysicalDeviceInformation getDeviceInformation() = 0;

  /**
   * @brief Get device stats
   *
   * @return Device stats
   */
  virtual const DeviceStats &getDeviceStats() const = 0;

  /**
   * @brief Destroy all resources in the device
   *
   * This does not destroy the device
   */
  virtual void destroyResources() = 0;

  /**
   * @brief Get swapchain
   *
   * @return Swapchain
   */
  virtual Swapchain getSwapchain() = 0;
};

} // namespace liquid::rhi
