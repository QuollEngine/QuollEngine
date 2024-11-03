#pragma once

#include "quoll/rhi/RenderDevice.h"

namespace quoll {

class BindlessDrawParameters {
  struct Range {
    usize offset = 0;
    usize size = 0;
    void *data = nullptr;
  };

public:
  BindlessDrawParameters(usize minBufferAlignment)
      : mMinBufferAlignment(minBufferAlignment) {}

  template <class TData> usize addRange(TData &&data) {
    usize structSize = sizeof(TData);
    auto *bytes = new TData;
    *bytes = data;

    usize currentOffset = mLastOffset;
    mRanges.push_back({currentOffset, structSize, bytes});

    mLastOffset += padSizeToMinimumUniformAlignment(structSize);
    return currentOffset;
  }

  inline const rhi::Descriptor &getDescriptor() const { return mDescriptor; }

  void build(rhi::RenderDevice *device);

  void destroy(rhi::RenderDevice *device);

private:
  usize padSizeToMinimumUniformAlignment(usize originalSize) const;

private:
  std::vector<Range> mRanges;
  usize mLastOffset = 0;
  usize mMinBufferAlignment = 0;

  rhi::Buffer mBuffer;
  rhi::DescriptorLayoutHandle mDescriptorLayout =
      rhi::DescriptorLayoutHandle::Null;
  rhi::Descriptor mDescriptor;
};

} // namespace quoll
