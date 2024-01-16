#pragma once

#include "quoll/rhi/BufferDescription.h"
#include "quoll/rhi/NativeBuffer.h"

namespace quoll::rhi {

class MockBuffer : public NativeBuffer {
public:
  MockBuffer(const BufferDescription &description);

  void *map() override;

  void unmap() override;

  void resize(usize size) override;

  rhi::DeviceAddress getAddress() override;

  inline const BufferDescription &getDescription() const {
    return mDescription;
  }

private:
  std::vector<u8> mData;
  BufferDescription mDescription;
};

} // namespace quoll::rhi
