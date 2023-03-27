#pragma once

#include "liquid/rhi/RenderBackend.h"

namespace liquid::rhi {

/**
 * @brief Mock render backend
 */
class MockRenderBackend : public RenderBackend {
  /**
   * @brief Get device with default parameters
   *
   * @return Render device
   */
  RenderDevice *createDefaultDevice() override;
};

} // namespace liquid::rhi
