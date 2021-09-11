#include <gtest/gtest.h>
#include "renderer/RenderCommandList.h"
#include "../mocks/TestResourceAllocator.h"

class RenderCommandListTests : public ::testing::Test {
public:
  TestResourceAllocator resourceAllocator;
};

class RenderCommandListDeathTest : public RenderCommandListTests {
public:
};

TEST_F(RenderCommandListTests, RecordBeginRenderPass) {
  liquid::RenderCommandList commandList;

  commandList.beginRenderPass((VkRenderPass)0x34343434, (VkFramebuffer)0x232322,
                              {0.5, 1.0}, {800, 600}, {});
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BeginRenderPass);
}

TEST_F(RenderCommandListTests, RecordEndRenderPass) {
  liquid::RenderCommandList commandList;

  commandList.endRenderPass();
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::EndRenderPass);
}

TEST_F(RenderCommandListTests, RecordBindPipeline) {
  liquid::RenderCommandList commandList;

  commandList.bindPipeline((VkPipeline)0x34343434,
                           VK_PIPELINE_BIND_POINT_GRAPHICS);
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BindPipeline);
}

TEST_F(RenderCommandListTests, RecordBindDescriptorSets) {
  liquid::RenderCommandList commandList;

  commandList.bindDescriptorSets((VkPipelineLayout)0x2323232,
                                 VK_PIPELINE_BIND_POINT_GRAPHICS, 0, {}, {});
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BindDescriptorSets);
}

TEST_F(RenderCommandListDeathTest, BindVertexBufferFailsIfBufferIsNotVertex) {
  liquid::RenderCommandList commandList;
  EXPECT_DEATH(
      commandList.bindVertexBuffer(resourceAllocator.createIndexBuffer(0)),
      ".*");
}

TEST_F(RenderCommandListTests, RecordBindVertexBuffer) {
  liquid::RenderCommandList commandList;

  commandList.bindVertexBuffer(resourceAllocator.createVertexBuffer(0));
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BindVertexBuffer);
}

TEST_F(RenderCommandListDeathTest, BindIndexBufferFailsIfBufferIsNotIndex) {
  liquid::RenderCommandList commandList;
  EXPECT_DEATH(
      commandList.bindIndexBuffer(resourceAllocator.createVertexBuffer(0),
                                  VK_INDEX_TYPE_UINT32),
      ".*");
}

TEST_F(RenderCommandListTests, RecordBindIndexBuffer) {
  liquid::RenderCommandList commandList;

  commandList.bindIndexBuffer(resourceAllocator.createIndexBuffer(0),
                              VK_INDEX_TYPE_UINT32);
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BindIndexBuffer);
}

TEST_F(RenderCommandListTests, RecordPushConstants) {
  liquid::RenderCommandList commandList;

  commandList.pushConstants((VkPipelineLayout)0x112121,
                            VK_SHADER_STAGE_VERTEX_BIT, 0, 0, nullptr);
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::PushConstants);
}

TEST_F(RenderCommandListTests, SetViewport) {
  liquid::RenderCommandList commandList;

  commandList.setViewport({0, 0}, {800, 600}, {0.0, 1.0});
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::SetViewport);
}

TEST_F(RenderCommandListTests, SetScissor) {
  liquid::RenderCommandList commandList;

  commandList.setScissor({0, 0}, {800, 600});
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::SetScissor);
}
