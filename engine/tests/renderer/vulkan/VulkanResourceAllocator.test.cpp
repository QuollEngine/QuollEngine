#include "core/Base.h"
#include "renderer/vulkan/VulkanResourceAllocator.h"
#include "renderer/vulkan/VulkanTextureBinder.h"
#include "renderer/vulkan/VulkanError.h"

#include <gtest/gtest.h>
#include "../../mocks/VulkanMock.h"

using ::testing::_;
using ::testing::Return;

class VulkanResourceAllocatorTests : public VulkanTestBase {
public:
};

TEST_F(VulkanResourceAllocatorTests, CreatesTexture2D) {

  unsigned char data[] = {1, 2, 3, 4, 5, 6, 7};

  EXPECT_CALL(*vmaLibMock, vmaMapMemory)
      .WillOnce([&data](auto allocator, auto allocation, void **retData) {
        *retData = data;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vmaLibMock, vmaCreateImage)
      .WillOnce([](auto allocator, const auto *imageCreateInfo,
                   const auto *imageAllocationInfo, auto image,
                   auto *allocation, auto *x) {
        *image = (VkImage)0xff00ff00;
        *allocation = (VmaAllocation)0xff00ffaa;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vulkanLibMock, vkCreateImageView)
      .WillOnce([](auto device, const auto *pCreateInfo, const auto *allocator,
                   auto *imageView) {
        *imageView = (VkImageView)0xff00ffcc;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vulkanLibMock, vkCreateSampler)
      .WillOnce([](auto device, const auto *pCreateInfo, const auto *allocator,
                   auto *sampler) {
        *sampler = (VkSampler)0xff00ffee;
        return VK_SUCCESS;
      });

  liquid::StatsManager statsManager;
  liquid::VulkanUploadContext uploadContext;
  liquid::VulkanResourceAllocator resourceAllocator(uploadContext, nullptr,
                                                    nullptr, statsManager);

  const auto &texture = resourceAllocator.createTexture2D({1, 7, 1, 0, data});

  EXPECT_NE(texture, nullptr);
  EXPECT_NE(texture->getResourceBinder(), nullptr);

  const auto &vulkanBinder =
      std::static_pointer_cast<liquid::VulkanTextureBinder>(
          texture->getResourceBinder());

  EXPECT_NE(vulkanBinder->image, nullptr);
  EXPECT_NE(vulkanBinder->imageView, nullptr);
  EXPECT_NE(vulkanBinder->sampler, nullptr);
  EXPECT_NE(vulkanBinder->allocation, nullptr);
}

TEST_F(VulkanResourceAllocatorTests, CreatesTextureCubemap) {
  constexpr size_t layerSize = 5 * 5 * 4 * 6;
  unsigned char item[layerSize] = {};
  unsigned char *outData = new unsigned char[layerSize * 6];

  unsigned char *data = item;

  EXPECT_CALL(*vmaLibMock, vmaMapMemory)
      .WillOnce([&outData](auto allocator, auto allocation, void **retData) {
        *retData = outData;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vmaLibMock, vmaCreateImage)
      .WillOnce([](auto allocator, const auto *imageCreateInfo,
                   const auto *imageAllocationInfo, auto image,
                   auto *allocation, auto *x) {
        *image = (VkImage)0xff00ff00;
        *allocation = (VmaAllocation)0xff00ffaa;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vulkanLibMock, vkCreateImageView)
      .WillOnce([](auto device, const auto *pCreateInfo, const auto *allocator,
                   auto *imageView) {
        *imageView = (VkImageView)0xff00ffcc;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vulkanLibMock, vkCreateSampler)
      .WillOnce([](auto device, const auto *pCreateInfo, const auto *allocator,
                   auto *sampler) {
        *sampler = (VkSampler)0xff00ffee;
        return VK_SUCCESS;
      });

  liquid::StatsManager statsManager;
  liquid::VulkanUploadContext uploadContext;
  liquid::VulkanResourceAllocator resourceAllocator(uploadContext, nullptr,
                                                    nullptr, statsManager);

  const auto &texture =
      resourceAllocator.createTextureCubemap({5, 5, 1, 0, data});

  EXPECT_NE(texture, nullptr);
  EXPECT_NE(texture->getResourceBinder(), nullptr);

  const auto &vulkanBinder =
      std::static_pointer_cast<liquid::VulkanTextureBinder>(
          texture->getResourceBinder());

  EXPECT_NE(vulkanBinder->image, nullptr);
  EXPECT_NE(vulkanBinder->imageView, nullptr);
  EXPECT_NE(vulkanBinder->sampler, nullptr);
  EXPECT_NE(vulkanBinder->allocation, nullptr);

  delete[] outData;
}

TEST_F(VulkanResourceAllocatorTests, CreatesTextureFramebuffer) {
  EXPECT_CALL(*vmaLibMock, vmaCreateImage)
      .WillOnce([](auto allocator, const VkImageCreateInfo *pCreateInfo,
                   const auto *imageAllocationInfo, auto image,
                   auto *allocation, auto *x) {
        *image = (VkImage)0xff00ff00;
        *allocation = (VmaAllocation)0xff00ffaa;

        EXPECT_EQ(pCreateInfo->arrayLayers, 5);
        EXPECT_EQ(pCreateInfo->format, VK_FORMAT_D16_UNORM);

        return VK_SUCCESS;
      });

  EXPECT_CALL(*vulkanLibMock, vkCreateImageView)
      .WillOnce([](auto device, const auto *pCreateInfo, const auto *allocator,
                   auto *imageView) {
        *imageView = (VkImageView)0xff00ffcc;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vulkanLibMock, vkCreateSampler)
      .WillOnce([](auto device, const auto *pCreateInfo, const auto *allocator,
                   auto *sampler) {
        *sampler = (VkSampler)0xff00ffee;
        return VK_SUCCESS;
      });

  liquid::StatsManager statsManager;
  liquid::VulkanUploadContext uploadContext;
  liquid::VulkanResourceAllocator resourceAllocator(uploadContext, nullptr,
                                                    nullptr, statsManager);

  liquid::TextureFramebufferData textureData{};
  textureData.width = 1024;
  textureData.height = 1024;
  textureData.layers = 5;
  textureData.format = VK_FORMAT_D16_UNORM;
  const auto &texture = resourceAllocator.createTextureFramebuffer(textureData);

  EXPECT_NE(texture, nullptr);
  EXPECT_NE(texture->getResourceBinder(), nullptr);

  const auto &vulkanBinder =
      std::static_pointer_cast<liquid::VulkanTextureBinder>(
          texture->getResourceBinder());

  EXPECT_NE(vulkanBinder->image, nullptr);
  EXPECT_NE(vulkanBinder->imageView, nullptr);
  EXPECT_NE(vulkanBinder->sampler, nullptr);
  EXPECT_NE(vulkanBinder->allocation, nullptr);
}
