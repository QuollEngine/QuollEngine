#include "liquid/core/Base.h"

#include "liquid/rhi/Buffer.h"
#include "liquid/rhi/NativeBuffer.h"

namespace quoll::rhi {

Buffer::Buffer(BufferHandle handle, NativeBuffer *nativeBuffer)
    : mHandle(handle), mNativeBuffer(nativeBuffer) {}

void *Buffer::map() { return mNativeBuffer->map(); }

void Buffer::unmap() { mNativeBuffer->unmap(); }

void Buffer::update(const void *data, size_t size) {
  auto *mappedData = map();
  memcpy(mappedData, data, size);
  unmap();
}

void Buffer::resize(size_t size) { mNativeBuffer->resize(size); }

} // namespace quoll::rhi
