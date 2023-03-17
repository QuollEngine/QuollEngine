#include "liquid/core/Base.h"
#include "BindlessDrawParameters.h"

namespace liquid {

void BindlessDrawParameters::build(rhi::RenderDevice *device) {
  // Create buffer
  {
    rhi::BufferDescription description{};
    description.size = mLastOffset;
    description.usage = rhi::BufferUsage::Uniform;
    description.data = nullptr;

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
    mDescriptor = device->createDescriptor(mDescriptorLayout);
    mDescriptor.write(0, {mBuffer.getHandle()},
                      rhi::DescriptorType::UniformBufferDynamic, 0);

    size_t maxSize = 0;
    for (auto &range : mRanges) {
      maxSize = std::max(range.size, maxSize);
    }

    rhi::DescriptorBufferInfo info{};
    info.offset = 0;
    info.range = static_cast<uint32_t>(maxSize);
    info.buffer = mBuffer.getHandle();

    mDescriptor.write(0, {info}, rhi::DescriptorType::UniformBufferDynamic, 0);
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

} // namespace liquid
