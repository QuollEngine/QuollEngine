#pragma once

#include "quoll/rhi/RenderHandle.h"
#include "quoll/rhi/NativeBuffer.h"

namespace quoll::rhi {

class Buffer {
public:
  Buffer() = default;

  Buffer(BufferHandle handle, NativeBuffer *nativeBuffer);

  void *map();

  void unmap();

  void update(const void *data, usize size);

  void resize(usize size);

  inline DeviceAddress getAddress() const {
    return mNativeBuffer->getAddress();
  }

  inline BufferHandle getHandle() const { return mHandle; }

private:
  BufferHandle mHandle = BufferHandle::Null;
  NativeBuffer *mNativeBuffer = nullptr;
};

} // namespace quoll::rhi
