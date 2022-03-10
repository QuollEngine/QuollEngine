#include "liquid/core/Base.h"
#include "VulkanTexture.h"

namespace liquid::experimental {

VulkanTexture::VulkanTexture(VkImage image, VkImageView imageView,
                             VkSampler sampler, VmaAllocation allocation)
    : mImage(image), mImageView(imageView), mSampler(sampler),
      mAllocation(allocation) {}

} // namespace liquid::experimental
