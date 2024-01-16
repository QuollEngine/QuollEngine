#pragma once

#include "quoll/rhi/RenderBackend.h"

namespace quoll::rhi {

class MockRenderBackend : public RenderBackend {
  RenderDevice *createDefaultDevice() override;
};

} // namespace quoll::rhi
