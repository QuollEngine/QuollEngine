#include "liquid/core/Base.h"
#include "MockBuffer.h"

namespace liquid::rhi {

MockBuffer::MockBuffer(const BufferDescription &description)
    : mDescription(description) {
  mData.resize(description.size);
  const auto *data = static_cast<const uint8_t *>(description.data);
  memcpy(mData.data(), data, description.size);
}

void *MockBuffer::map() { return mData.data(); }

void MockBuffer::unmap() {
  // Do nothing as the data is always mapped
}

void MockBuffer::resize(size_t size) { mData.resize(size); }

} // namespace liquid::rhi
