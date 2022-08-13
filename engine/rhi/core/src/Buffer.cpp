#include "liquid/core/Base.h"

#include "liquid/rhi/Buffer.h"
#include "liquid/rhi/NativeBuffer.h"

namespace liquid::rhi {

Buffer::Buffer(BufferHandle handle, NativeBuffer *nativeBuffer)
    : mHandle(handle), mNativeBuffer(nativeBuffer) {}

void *Buffer::map() { return mNativeBuffer->map(); }

void Buffer::unmap() { mNativeBuffer->unmap(); }

void Buffer::update(void *data) { mNativeBuffer->update(data); }

void Buffer::resize(size_t size) { mNativeBuffer->resize(size); }

} // namespace liquid::rhi
