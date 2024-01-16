#pragma once

#include "RenderDevice.h"

namespace quoll::rhi {

class RenderBackend {
public:
  RenderBackend(const RenderBackend &) = delete;
  RenderBackend &operator=(const RenderBackend &) = delete;
  RenderBackend(RenderBackend &&) = delete;
  RenderBackend &operator=(RenderBackend &&) = delete;

  RenderBackend() = default;

  virtual ~RenderBackend() = default;

  virtual RenderDevice *createDefaultDevice() = 0;
};

} // namespace quoll::rhi
