#include <gtest/gtest.h>
#include "../../mocks/TestResourceAllocator.h"
#include "../../mocks/VulkanMock.h"
#include "renderer/vulkan/VulkanShadowPass.h"
#include "renderer/vulkan/VulkanError.h"
#include "renderer/vulkan/VulkanTextureBinder.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SaveArg;

class VulkanTestResourceAllocator : public TestResourceAllocator {
public:
  liquid::SharedPtr<liquid::Texture>
  createTextureFramebuffer(const liquid::TextureFramebufferData &data) {
    auto binder = std::make_shared<liquid::VulkanTextureBinder>(
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    return std::make_shared<liquid::Texture>(binder, 0, nullptr);
  }
};

class VulkanShadowPassTests : public VulkanTestBase {
public:
  VulkanTestResourceAllocator resourceAllocator;
};

TEST_F(VulkanShadowPassTests, ThrowsErrorIfCreateRenderPassFails) {
  ON_CALL(*vulkanLibMock, vkCreateRenderPass)
      .WillByDefault(Return(VK_ERROR_UNKNOWN));
  ON_CALL(*vulkanLibMock, vkCreateFramebuffer)
      .WillByDefault(Return(VK_SUCCESS));

  EXPECT_THROW(
      {
        liquid::VulkanShadowPass shadowPass(2048, nullptr, nullptr, nullptr,
                                            &resourceAllocator, nullptr,
                                            nullptr);
      },
      liquid::VulkanError);
}

TEST_F(VulkanShadowPassTests, CreatesRenderPass) {
  ON_CALL(*vulkanLibMock, vkCreateRenderPass).WillByDefault(Return(VK_SUCCESS));

  EXPECT_CALL(*vulkanLibMock, vkCreateRenderPass(_, _, _, _)).Times(1);

  liquid::VulkanShadowPass shadowPass(2048, nullptr, nullptr, nullptr,
                                      &resourceAllocator, nullptr, nullptr);

  EXPECT_EQ(shadowPass.getExtent().width, 2048);
  EXPECT_EQ(shadowPass.getExtent().height, 2048);
}

TEST_F(VulkanShadowPassTests, CreatesResourceManager) {
  ON_CALL(*vulkanLibMock, vkCreateRenderPass).WillByDefault(Return(VK_SUCCESS));
  ON_CALL(*vulkanLibMock, vkCreateFramebuffer)
      .WillByDefault(Return(VK_SUCCESS));
  liquid::VulkanShadowPass shadowPass(2048, nullptr, nullptr, nullptr,
                                      &resourceAllocator, nullptr, nullptr);

  EXPECT_NE(shadowPass.getResourceManager(), nullptr);
}

TEST_F(VulkanShadowPassTests, CreatesShadowmapTextures) {
  liquid::VulkanShadowPass shadowPass(2048, nullptr, nullptr, nullptr,
                                      &resourceAllocator, nullptr, nullptr);

  EXPECT_NE(shadowPass.getShadowmap(), nullptr);
}

TEST_F(VulkanShadowPassTests, ThrowsErrorIfCreateFramebufferFails) {
  ON_CALL(*vulkanLibMock, vkCreateRenderPass).WillByDefault(Return(VK_SUCCESS));
  ON_CALL(*vulkanLibMock, vkCreateFramebuffer)
      .WillByDefault(Return(VK_ERROR_UNKNOWN));

  EXPECT_THROW(
      {
        liquid::VulkanShadowPass shadowPass(2048, nullptr, nullptr, nullptr,
                                            &resourceAllocator, nullptr,
                                            nullptr);
      },
      liquid::VulkanError);
}

TEST_F(VulkanShadowPassTests, CreatesFramebuffers) {
  ON_CALL(*vulkanLibMock, vkCreateRenderPass).WillByDefault(Return(VK_SUCCESS));
  ON_CALL(*vulkanLibMock, vkCreateFramebuffer)
      .WillByDefault(Return(VK_SUCCESS));

  EXPECT_CALL(*vulkanLibMock, vkCreateFramebuffer(_, _, _, _)).Times(1);

  liquid::VulkanShadowPass shadowPass(2048, nullptr, nullptr, nullptr,
                                      &resourceAllocator, nullptr, nullptr);
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

  VkRenderPassBeginInfo *beginInfo = new VkRenderPassBeginInfo;
  EXPECT_CALL(*vulkanLibMock,
              vkCmdBeginRenderPass(_, _, VK_SUBPASS_CONTENTS_INLINE))
      .Times(1)
      .WillOnce([&beginInfo](VkCommandBuffer commandBuffer,
                             const VkRenderPassBeginInfo *bi,
                             VkSubpassContents contents) {
        memcpy(beginInfo, bi, sizeof(VkRenderPassBeginInfo));
        VkClearValue *clearValue = new VkClearValue[bi->clearValueCount];
        memcpy(clearValue, bi->pClearValues,
               sizeof(VkClearValue) * bi->clearValueCount);
        beginInfo->pClearValues = clearValue;
      });

  EXPECT_CALL(*vulkanLibMock, vkCmdEndRenderPass(_)).Times(1);

  liquid::VulkanShadowPass shadowPass(4096, nullptr, nullptr, nullptr,
                                      &resourceAllocator, nullptr, nullptr);

  VkCommandBuffer fakeBuffer = (VkCommandBuffer)0x82323232;
  shadowPass.render(fakeBuffer, [fakeBuffer](VkCommandBuffer buffer) {
    EXPECT_EQ(fakeBuffer, buffer);
  });

  EXPECT_EQ(beginInfo->framebuffer, fakeFramebuffer);
  EXPECT_EQ(beginInfo->renderPass, fakeRenderPass);
  EXPECT_EQ(beginInfo->renderArea.extent.width, 4096);
  EXPECT_EQ(beginInfo->renderArea.extent.height, 4096);
  EXPECT_EQ(beginInfo->renderArea.offset.x, 0);
  EXPECT_EQ(beginInfo->renderArea.offset.y, 0);
  EXPECT_EQ(beginInfo->clearValueCount, 1);
  EXPECT_EQ(beginInfo->pClearValues[0].depthStencil.depth, 1.0f);
  EXPECT_EQ(beginInfo->pClearValues[0].depthStencil.stencil, 0);
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
  liquid::VulkanShadowPass shadowPass(2048, nullptr, nullptr, nullptr,
                                      &resourceAllocator, nullptr, nullptr);
}
