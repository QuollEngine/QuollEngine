#pragma once

#include "RenderDevice.h"

namespace quoll::rhi {

class RenderBackend : NoCopyMove {
public:
  RenderBackend() = default;

  virtual ~RenderBackend() = default;

  virtual RenderDevice *createDefaultDevice() = 0;
};

} // namespace quoll::rhi
