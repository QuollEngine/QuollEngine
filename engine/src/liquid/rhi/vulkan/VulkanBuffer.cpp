#include "liquid/core/Base.h"
#include "VulkanBuffer.h"

namespace liquid::experimental {

VulkanBuffer::VulkanBuffer(BufferType type, size_t size, VkBuffer buffer,
                           VmaAllocation allocation)
    : mType(type), mSize(size), mBuffer(buffer), mAllocation(allocation) {}

} // namespace liquid::experimental
