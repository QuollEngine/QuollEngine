#include "quoll/core/Base.h"
#include "BindlessDrawParameters.h"

namespace quoll {

void BindlessDrawParameters::build(rhi::RenderDevice *device) {
  usize maxSize = 0;
  for (const auto &range : mRanges) {
    maxSize = std::max(range.size, maxSize);
  }

  maxSize = padSizeToMinimumUniformAlignment(maxSize);

  // Create buffer
  {
    rhi::BufferDescription description{};
    description.size = mLastOffset + maxSize;
    description.usage = rhi::BufferUsage::Uniform;
    description.data = nullptr;
    description.debugName = "Bindless draw parameters";

    mBuffer = device->createBuffer(description);

    u8 *data = static_cast<u8 *>(mBuffer.map());
    for (const auto &range : mRanges) {
      memcpy(data + range.offset, range.data, range.size);
    }

    mBuffer.unmap();
  }

  // Create descriptor layout
  {
    rhi::DescriptorLayoutBindingDescription description{};
    description.name = "uDrawParams";
    description.binding = 0;
    description.descriptorType = rhi::DescriptorType::UniformBufferDynamic;
    description.descriptorCount = 1;
    description.shaderStage = rhi::ShaderStage::All;
    mDescriptorLayout = device->createDescriptorLayout({{description}});
  }

  // Create descriptor
  {
    std::array<rhi::BufferHandle, 1> buffers{mBuffer.getHandle()};
    mDescriptor = device->createDescriptor(mDescriptorLayout);
    mDescriptor.write(0, buffers, rhi::DescriptorType::UniformBufferDynamic, 0);

    rhi::DescriptorBufferInfo info{};
    info.offset = 0;
    info.range = static_cast<u32>(maxSize);
    info.buffer = mBuffer.getHandle();

    std::array<rhi::DescriptorBufferInfo, 1> bufferInfos{info};
    mDescriptor.write(0, bufferInfos, rhi::DescriptorType::UniformBufferDynamic,
                      0);
  }
}

void BindlessDrawParameters::destroy(rhi::RenderDevice *device) {
  mRanges.clear();
  mLastOffset = 0;
  if (rhi::isHandleValid(mBuffer.getHandle())) {
    device->destroyBuffer(mBuffer.getHandle());
    mBuffer = rhi::Buffer();
  }
}

usize BindlessDrawParameters::padSizeToMinimumUniformAlignment(
    usize originalSize) const {
  if (mMinBufferAlignment > 0) {
    return (originalSize + mMinBufferAlignment - 1) &
           ~(mMinBufferAlignment - 1);
  }

  return originalSize;
}

} // namespace quoll
