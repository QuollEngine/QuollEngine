#include "quoll/core/Base.h"

#include "quoll/rhi/Buffer.h"
#include "quoll/rhi/NativeBuffer.h"

namespace quoll::rhi {

Buffer::Buffer(BufferHandle handle, NativeBuffer *nativeBuffer)
    : mHandle(handle), mNativeBuffer(nativeBuffer) {}

void *Buffer::map() { return mNativeBuffer->map(); }

void Buffer::unmap() { mNativeBuffer->unmap(); }

void Buffer::update(const void *data, usize size) {
  auto *mappedData = map();
  memcpy(mappedData, data, size);
  unmap();
}

void Buffer::resize(usize size) { mNativeBuffer->resize(size); }

} // namespace quoll::rhi
