#include "core/Base.h"
#include "renderer/vulkan/VulkanDescriptorManager.h"
#include "renderer/vulkan/VulkanError.h"
#include "renderer/vulkan/VulkanHardwareBuffer.h"
#include "renderer/vulkan/VulkanTextureBinder.h"

#include <gtest/gtest.h>
#include "../../mocks/VulkanMock.h"
#include "../../mocks/TestResourceAllocator.h"

using ::testing::_;
using ::testing::Return;

class VulkanDescriptorManagerTests : public VulkanTestBase {
public:
  liquid::StatsManager statsManager;
};
TEST_F(VulkanDescriptorManagerTests, ThrowsErrorIfCreateDescriptorPoolFails) {
  ON_CALL(*vulkanLibMock, vkCreateDescriptorSetLayout)
      .WillByDefault(Return(VK_SUCCESS));
  ON_CALL(*vulkanLibMock, vkCreateDescriptorPool)
      .WillByDefault(Return(VK_ERROR_UNKNOWN));

  EXPECT_THROW({ liquid::VulkanDescriptorManager manager(nullptr); },
               liquid::VulkanError);
}

TEST_F(VulkanDescriptorManagerTests, CreatesDescriptorLayoutsAndPool) {
  ON_CALL(*vulkanLibMock, vkCreateDescriptorPool)
      .WillByDefault(Return(VK_SUCCESS));

  // Create descriptor pool
  EXPECT_CALL(*vulkanLibMock, vkCreateDescriptorPool(_, _, _, _)).Times(1);

  EXPECT_NO_THROW({ liquid::VulkanDescriptorManager manager(nullptr); });
}

TEST_F(VulkanDescriptorManagerTests,
       ThrowsErrorIfSceneDescriptorSetAllocationFails) {
  ON_CALL(*vulkanLibMock, vkAllocateDescriptorSets)
      .WillByDefault(Return(VK_ERROR_UNKNOWN));

  liquid::VulkanDescriptorManager manager(nullptr);
  EXPECT_THROW(
      {
        manager.createSceneDescriptorSet(nullptr, nullptr, nullptr, {},
                                         (VkDescriptorSetLayout)0x232323232);
      },
      liquid::VulkanError);
}

TEST_F(VulkanDescriptorManagerTests, CreatesSceneDescriptorSetWithShadowmaps) {
  ON_CALL(*vulkanLibMock, vkAllocateDescriptorSets)
      .WillByDefault(Return(VK_SUCCESS));

  EXPECT_CALL(*vulkanLibMock, vkUpdateDescriptorSets(_, 3, _, _, _)).Times(1);

  liquid::SharedPtr<liquid::Texture> shadowmaps =
      std::make_shared<liquid::Texture>(
          std::make_shared<liquid::VulkanTextureBinder>(
              nullptr, nullptr, nullptr, nullptr, nullptr, nullptr),
          0, statsManager);

  liquid::VulkanDescriptorManager manager(nullptr);
  manager.createSceneDescriptorSet(
      std::make_shared<liquid::VulkanHardwareBuffer>(
          liquid::HardwareBuffer::Uniform, 0, nullptr, nullptr, nullptr,
          statsManager),
      std::make_shared<liquid::VulkanHardwareBuffer>(
          liquid::HardwareBuffer::Uniform, 0, nullptr, nullptr, nullptr,
          statsManager),
      shadowmaps, {}, (VkDescriptorSetLayout)0x232323232);
}

TEST_F(VulkanDescriptorManagerTests,
       CreatesSceneDescriptorSetWithEnvironmentMaps) {
  ON_CALL(*vulkanLibMock, vkAllocateDescriptorSets)
      .WillByDefault(Return(VK_SUCCESS));

  EXPECT_CALL(*vulkanLibMock, vkUpdateDescriptorSets(_, 5, _, _, _)).Times(1);

  liquid::SharedPtr<liquid::Texture> sampleTexture =
      std::make_shared<liquid::Texture>(
          std::make_shared<liquid::VulkanTextureBinder>(
              nullptr, nullptr, nullptr, nullptr, nullptr, nullptr),
          0, statsManager);

  liquid::VulkanDescriptorManager manager(nullptr);
  manager.createSceneDescriptorSet(
      std::make_shared<liquid::VulkanHardwareBuffer>(
          liquid::HardwareBuffer::Uniform, 0, nullptr, nullptr, nullptr,
          statsManager),
      std::make_shared<liquid::VulkanHardwareBuffer>(
          liquid::HardwareBuffer::Uniform, 0, nullptr, nullptr, nullptr,
          statsManager),
      sampleTexture, {sampleTexture, sampleTexture, sampleTexture},
      (VkDescriptorSetLayout)0x232323232);
}

TEST_F(VulkanDescriptorManagerTests,
       ThrowsErrorIfMaterialTexturesSizeIsMoreThanLimit) {
  liquid::VulkanDescriptorManager manager(nullptr);
  // MAX_TEXTURE_DESCRIPTORS = 8
  std::vector<liquid::SharedPtr<liquid::Texture>> textures(9);

  EXPECT_THROW({ manager.createMaterialDescriptorSet(nullptr, textures); },
               liquid::VulkanError);
}

TEST_F(VulkanDescriptorManagerTests,
       ThrowsErrorIfMaterialDescriptorSetAllocationFails) {
  ON_CALL(*vulkanLibMock, vkAllocateDescriptorSets)
      .WillByDefault(Return(VK_ERROR_UNKNOWN));
  liquid::VulkanDescriptorManager manager(nullptr);

  EXPECT_THROW({ manager.createMaterialDescriptorSet(nullptr, {}); },
               liquid::VulkanError);
}

TEST_F(VulkanDescriptorManagerTests,
       CreatesMaterialDescriptorSetWithOnlyBuffer) {
  ON_CALL(*vulkanLibMock, vkAllocateDescriptorSets)
      .WillByDefault(Return(VK_SUCCESS));

  EXPECT_CALL(*vulkanLibMock, vkUpdateDescriptorSets(_, 1, _, _, _)).Times(1);

  liquid::VulkanDescriptorManager manager(nullptr);
  manager.createMaterialDescriptorSet(
      std::make_shared<liquid::VulkanHardwareBuffer>(
          liquid::HardwareBuffer::Uniform, 0, nullptr, nullptr, nullptr,
          statsManager),
      {});
}

TEST_F(VulkanDescriptorManagerTests,
       CreatesMaterialDescriptorSetWithOnlyTextures) {
  ON_CALL(*vulkanLibMock, vkAllocateDescriptorSets)
      .WillByDefault(Return(VK_SUCCESS));

  EXPECT_CALL(*vulkanLibMock, vkUpdateDescriptorSets(_, 1, _, _, _)).Times(1);

  liquid::VulkanDescriptorManager manager(nullptr);

  manager.createMaterialDescriptorSet(
      nullptr, {std::make_shared<liquid::Texture>(
                   std::make_shared<liquid::VulkanTextureBinder>(
                       nullptr, nullptr, nullptr, nullptr, nullptr, nullptr),
                   0, statsManager)});
}

TEST_F(VulkanDescriptorManagerTests,
       CreatesMaterialDescriptorSetWithBufferAndTextures) {
  ON_CALL(*vulkanLibMock, vkAllocateDescriptorSets)
      .WillByDefault(Return(VK_SUCCESS));

  EXPECT_CALL(*vulkanLibMock, vkUpdateDescriptorSets(_, 2, _, _, _)).Times(1);

  liquid::VulkanDescriptorManager manager(nullptr);
  manager.createMaterialDescriptorSet(
      std::make_shared<liquid::VulkanHardwareBuffer>(
          liquid::HardwareBuffer::Uniform, 0, nullptr, nullptr, nullptr,
          statsManager),
      {std::make_shared<liquid::Texture>(
          std::make_shared<liquid::VulkanTextureBinder>(
              nullptr, nullptr, nullptr, nullptr, nullptr, nullptr),
          0, statsManager)});
}

TEST_F(VulkanDescriptorManagerTests, DestroysDescriptorPoolIfExists) {
  ON_CALL(*vulkanLibMock, vkCreateDescriptorSetLayout)
      .WillByDefault(Return(VK_SUCCESS));
  VkDescriptorPool fakeValue = (VkDescriptorPool)0xfffffff;

  EXPECT_CALL(*vulkanLibMock, vkCreateDescriptorPool)
      .WillOnce([fakeValue](VkDevice, const VkDescriptorPoolCreateInfo *,
                            const VkAllocationCallbacks *,
                            VkDescriptorPool *pDescriptorPool) {
        *pDescriptorPool = fakeValue;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vulkanLibMock, vkDestroyDescriptorPool(_, fakeValue, _))
      .Times(1);
  liquid::VulkanDescriptorManager manager(nullptr);
}
