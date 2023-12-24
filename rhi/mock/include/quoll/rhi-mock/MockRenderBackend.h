#pragma once

#include "quoll/rhi/RenderBackend.h"

namespace quoll::rhi {

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

} // namespace quoll::rhi
