#include "liquid/core/Base.h"
#include "MockRenderBackend.h"
#include "MockRenderDevice.h"

namespace quoll::rhi {

RenderDevice *MockRenderBackend::createDefaultDevice() {
  return new MockRenderDevice;
}

} // namespace quoll::rhi
