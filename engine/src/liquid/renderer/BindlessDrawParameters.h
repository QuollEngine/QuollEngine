#pragma once

#include "liquid/rhi/RenderDevice.h"

namespace liquid {

/**
 * @brief Bindless draw parameters
 */
class BindlessDrawParameters {
  struct Range {
    size_t offset = 0;
    size_t size = 0;
    void *data = nullptr;
  };

public:
  /**
   * @brief Create bindless draw parameters
   *
   * @param minBufferAlignment Minimum buffer alignment
   */
  BindlessDrawParameters(size_t minBufferAlignment)
      : mMinBufferAlignment(minBufferAlignment) {}

  /**
   * @brief Add range to registry
   *
   * @tparam TData Data type
   * @param data Data
   * @return Range offset
   */
  template <class TData> size_t addRange(TData &&data) {
    size_t structSize = sizeof(TData);
    auto *bytes = new TData;
    *bytes = data;

    size_t currentOffset = mLastOffset;
    mRanges.push_back({currentOffset, structSize, bytes});

    mLastOffset += padSizeToMinimumUniformAlignment(structSize);
    return currentOffset;
  }

  /**
   * @brief Get descriptor
   *
   * @return Descriptor
   */
  inline const rhi::Descriptor &getDescriptor() const { return mDescriptor; }

  /**
   * @brief Build buffers and descriptor
   *
   * @param device Render device
   */
  void build(rhi::RenderDevice *device);

private:
  /**
   * @brief Pad size to minimum uniform alignment
   *
   * @param originalSize Original size
   * @return Padded size
   */
  size_t padSizeToMinimumUniformAlignment(size_t originalSize);

private:
  std::vector<Range> mRanges;
  size_t mLastOffset = 0;
  size_t mMinBufferAlignment = 0;

  rhi::Buffer mBuffer;
  rhi::DescriptorLayoutHandle mDescriptorLayout =
      rhi::DescriptorLayoutHandle::Invalid;
  rhi::Descriptor mDescriptor;
};

} // namespace liquid
