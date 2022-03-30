#pragma once

#include "liquid/profiler/PhysicalDeviceInformation.h"

#include "DeviceStats.h"
#include "RenderGraph.h"
#include "RenderGraphEvaluator.h"

namespace liquid::rhi {

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
   * @brief Execute render graph
   *
   * @param graph Render graph
   * @param evaluator Render graph evaluator
   */
  virtual void execute(RenderGraph &graph, RenderGraphEvaluator &evaluator) = 0;

  /**
   * @brief Wait for device to be idle
   */
  virtual void waitForIdle() = 0;

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
};

} // namespace liquid::rhi
