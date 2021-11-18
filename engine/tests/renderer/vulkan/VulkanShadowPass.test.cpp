#include "core/Base.h"
#include "renderer/vulkan/VulkanShadowPass.h"
#include "renderer/vulkan/VulkanError.h"
#include "renderer/vulkan/VulkanTextureBinder.h"

#include <gtest/gtest.h>
#include "../../mocks/TestResourceAllocator.h"
#include "../../mocks/VulkanMock.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SaveArg;

class VulkanTestResourceAllocator : public TestResourceAllocator {
public:
  liquid::SharedPtr<liquid::Texture>
  createTextureFramebuffer(const liquid::TextureFramebufferData &data) {
    auto binder = std::make_shared<liquid::VulkanTextureBinder>(
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    return std::make_shared<liquid::Texture>(binder, 0, statsManager);
  }
};

class VulkanShadowPassTests : public VulkanTestBase {
public:
  VulkanTestResourceAllocator resourceAllocator;
  liquid::StatsManager statsManager;
};

TEST_F(VulkanShadowPassTests, ThrowsErrorIfCreateRenderPassFails) {
  ON_CALL(*vulkanLibMock, vkCreateRenderPass)
      .WillByDefault(Return(VK_ERROR_UNKNOWN));
  ON_CALL(*vulkanLibMock, vkCreateFramebuffer)
      .WillByDefault(Return(VK_SUCCESS));

  EXPECT_THROW(
      {
        liquid::VulkanShadowPass shadowPass(
            2048, nullptr, nullptr, &resourceAllocator, nullptr, statsManager);
      },
      liquid::VulkanError);
}

TEST_F(VulkanShadowPassTests, CreatesRenderPass) {
  ON_CALL(*vulkanLibMock, vkCreateRenderPass).WillByDefault(Return(VK_SUCCESS));

  EXPECT_CALL(*vulkanLibMock, vkCreateRenderPass(_, _, _, _)).Times(1);

  liquid::VulkanShadowPass shadowPass(
      2048, nullptr, nullptr, &resourceAllocator, nullptr, statsManager);

  EXPECT_EQ(shadowPass.getExtent().width, 2048);
  EXPECT_EQ(shadowPass.getExtent().height, 2048);
}

TEST_F(VulkanShadowPassTests, CreatesResourceManager) {
  ON_CALL(*vulkanLibMock, vkCreateRenderPass).WillByDefault(Return(VK_SUCCESS));
  ON_CALL(*vulkanLibMock, vkCreateFramebuffer)
      .WillByDefault(Return(VK_SUCCESS));
  liquid::VulkanShadowPass shadowPass(
      2048, nullptr, nullptr, &resourceAllocator, nullptr, statsManager);

  EXPECT_NE(shadowPass.getResourceManager(), nullptr);
}

TEST_F(VulkanShadowPassTests, CreatesShadowmapTextures) {
  liquid::VulkanShadowPass shadowPass(
      2048, nullptr, nullptr, &resourceAllocator, nullptr, statsManager);

  EXPECT_NE(shadowPass.getShadowmap(), nullptr);
}

TEST_F(VulkanShadowPassTests, ThrowsErrorIfCreateFramebufferFails) {
  ON_CALL(*vulkanLibMock, vkCreateRenderPass).WillByDefault(Return(VK_SUCCESS));
  ON_CALL(*vulkanLibMock, vkCreateFramebuffer)
      .WillByDefault(Return(VK_ERROR_UNKNOWN));

  EXPECT_THROW(
      {
        liquid::VulkanShadowPass shadowPass(
            2048, nullptr, nullptr, &resourceAllocator, nullptr, statsManager);
      },
      liquid::VulkanError);
}

TEST_F(VulkanShadowPassTests, CreatesFramebuffers) {
  ON_CALL(*vulkanLibMock, vkCreateRenderPass).WillByDefault(Return(VK_SUCCESS));
  ON_CALL(*vulkanLibMock, vkCreateFramebuffer)
      .WillByDefault(Return(VK_SUCCESS));

  EXPECT_CALL(*vulkanLibMock, vkCreateFramebuffer(_, _, _, _)).Times(1);

  liquid::VulkanShadowPass shadowPass(
      2048, nullptr, nullptr, &resourceAllocator, nullptr, statsManager);
}

TEST_F(VulkanShadowPassTests, RenderCommandsInRenderPass) {
  VkFramebuffer fakeFramebuffer = (VkFramebuffer)0xffffffff;
  VkRenderPass fakeRenderPass = (VkRenderPass)0xffffff00;
  EXPECT_CALL(*vulkanLibMock, vkCreateRenderPass)
      .WillOnce([fakeRenderPass](VkDevice, const VkRenderPassCreateInfo *,
                                 const VkAllocationCallbacks *,
                                 VkRenderPass *pRenderPass) {
        *pRenderPass = fakeRenderPass;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vulkanLibMock, vkCreateFramebuffer)
      .WillOnce([fakeFramebuffer](VkDevice, const VkFramebufferCreateInfo *,
                                  const VkAllocationCallbacks *,
                                  VkFramebuffer *pFramebuffer) {
        *pFramebuffer = fakeFramebuffer;
        return VK_SUCCESS;
      });

  liquid::VulkanShadowPass shadowPass(
      4096, nullptr, nullptr, &resourceAllocator, nullptr, statsManager);

  liquid::RenderCommandList commandList;

  shadowPass.render(commandList,
                    [&commandList](liquid::RenderCommandList &cmdList) {
                      EXPECT_EQ(&commandList, &cmdList);
                    });

  EXPECT_EQ(commandList.getRecordedCommands().size(), 2);
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BeginRenderPass);
  EXPECT_EQ(commandList.getRecordedCommands().at(1)->type,
            liquid::RenderCommandType::EndRenderPass);

  auto *beginRenderPass = static_cast<liquid::RenderCommandBeginRenderPass *>(
      commandList.getRecordedCommands().at(0).get());
  EXPECT_EQ(beginRenderPass->framebuffer, fakeFramebuffer);
  EXPECT_EQ(beginRenderPass->renderPass, fakeRenderPass);
  EXPECT_TRUE(beginRenderPass->renderAreaSize == glm::uvec2(4096, 4096));
  EXPECT_TRUE(beginRenderPass->renderAreaOffset == glm::ivec2(0, 0));
  EXPECT_EQ(beginRenderPass->clearValues.size(), 1);
  EXPECT_EQ(beginRenderPass->clearValues.at(0).depthStencil.depth, 1.0f);
  EXPECT_EQ(beginRenderPass->clearValues.at(0).depthStencil.stencil, 0);
}

TEST_F(VulkanShadowPassTests, DestroysRenderPassAndFramebuffers) {
  EXPECT_CALL(*vulkanLibMock, vkCreateRenderPass)
      .WillOnce([](VkDevice, const VkRenderPassCreateInfo *,
                   const VkAllocationCallbacks *, VkRenderPass *pRenderPass) {
        *pRenderPass = (VkRenderPass)0xffffffff;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vulkanLibMock, vkCreateFramebuffer)
      .WillOnce([](VkDevice, const VkFramebufferCreateInfo *,
                   const VkAllocationCallbacks *, VkFramebuffer *pFramebuffer) {
        *pFramebuffer = (VkFramebuffer)0xffffffff;
        return VK_SUCCESS;
      });

  EXPECT_CALL(*vulkanLibMock, vkDestroyRenderPass(_, _, _)).Times(1);
  EXPECT_CALL(*vulkanLibMock, vkDestroyFramebuffer(_, _, _)).Times(1);
  liquid::VulkanShadowPass shadowPass(
      2048, nullptr, nullptr, &resourceAllocator, nullptr, statsManager);
}
