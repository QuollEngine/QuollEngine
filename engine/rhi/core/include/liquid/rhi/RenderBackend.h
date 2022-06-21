#pragma once

#include "RenderDevice.h"

namespace liquid::rhi {

/**
 * @brief Render backend
 */
class RenderBackend {
public:
  RenderBackend(const RenderBackend &) = delete;
  RenderBackend &operator=(const RenderBackend &) = delete;
  RenderBackend(RenderBackend &&) = delete;
  RenderBackend &operator=(RenderBackend &&) = delete;

  /**
   * @brief Default constructor
   */
  RenderBackend() = default;

  /**
   * @brief Destroy render backend
   */
  virtual ~RenderBackend() = default;

  /**
   * @brief Get device with default parameters
   *
   * @return Render device
   */
  virtual RenderDevice *createDefaultDevice() = 0;
};

} // namespace liquid::rhi
