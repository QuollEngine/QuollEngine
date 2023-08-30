#include "liquid/core/Base.h"
#include "BindlessDrawParameters.h"

namespace quoll {

void BindlessDrawParameters::build(rhi::RenderDevice *device) {
  size_t maxSize = 0;
  for (auto &range : mRanges) {
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

    uint8_t *data = static_cast<uint8_t *>(mBuffer.map());
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
    info.range = static_cast<uint32_t>(maxSize);
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

size_t
BindlessDrawParameters::padSizeToMinimumUniformAlignment(size_t originalSize) {
  if (mMinBufferAlignment > 0) {
    return (originalSize + mMinBufferAlignment - 1) &
           ~(mMinBufferAlignment - 1);
  }

  return originalSize;
}

} // namespace quoll
