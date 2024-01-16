#pragma once

#include "quoll/rhi/DeviceAddress.h"

namespace quoll::rhi {

class NativeBuffer {
public:
  virtual ~NativeBuffer() = default;

  virtual void *map() = 0;

  virtual void unmap() = 0;

  virtual void resize(usize size) = 0;

  virtual DeviceAddress getAddress() = 0;
};

} // namespace quoll::rhi
