#pragma once

#include "quoll/rhi/RenderDevice.h"

namespace quoll {

/**
 * @brief Bindless draw parameters
 */
class BindlessDrawParameters {
  struct Range {
    usize offset = 0;
    usize size = 0;
    void *data = nullptr;
  };

public:
  /**
   * @brief Create bindless draw parameters
   *
   * @param minBufferAlignment Minimum buffer alignment
   */
  BindlessDrawParameters(usize minBufferAlignment)
      : mMinBufferAlignment(minBufferAlignment) {}

  /**
   * @brief Add range to registry
   *
   * @tparam TData Data type
   * @param data Data
   * @return Range offset
   */
  template <class TData> usize addRange(TData &&data) {
    usize structSize = sizeof(TData);
    auto *bytes = new TData;
    *bytes = data;

    usize currentOffset = mLastOffset;
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

  /**
   * @brief Destroy buffers and descriptor
   *
   * @param device Render device
   */
  void destroy(rhi::RenderDevice *device);

private:
  /**
   * @brief Pad size to minimum uniform alignment
   *
   * @param originalSize Original size
   * @return Padded size
   */
  usize padSizeToMinimumUniformAlignment(usize originalSize);

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
