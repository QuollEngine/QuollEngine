#include "quoll/core/Base.h"
#include "MockBuffer.h"

namespace quoll::rhi {

MockBuffer::MockBuffer(const BufferDescription &description)
    : mDescription(description) {
  mData.resize(description.size);
  const auto *data = static_cast<const u8 *>(description.data);
  memcpy(mData.data(), data, description.size);
}

void *MockBuffer::map() { return mData.data(); }

void MockBuffer::unmap() {
  // Do nothing as the data is always mapped
}

void MockBuffer::resize(usize size) { mData.resize(size); }

rhi::DeviceAddress MockBuffer::getAddress() {
  return rhi::DeviceAddress{reinterpret_cast<u64>(this)};
}

} // namespace quoll::rhi
