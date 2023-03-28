#include "liquid/core/Base.h"
#include "MockRenderBackend.h"
#include "MockRenderDevice.h"

namespace liquid::rhi {

RenderDevice *MockRenderBackend::createDefaultDevice() {
  return new MockRenderDevice;
}

} // namespace liquid::rhi
