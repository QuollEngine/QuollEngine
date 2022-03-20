#pragma once

#include "liquid/renderer/render-graph/RenderGraph.h"
#include "liquid/renderer/render-graph/RenderGraphEvaluator.h"
#include "liquid/profiler/PhysicalDeviceInformation.h"

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
};

} // namespace liquid::rhi
