#include <gtest/gtest.h>
#include "renderer/RenderCommandList.h"
#include "../mocks/TestBuffer.h"

TEST(RenderCommandListTests, RecordBeginRenderPass) {
  liquid::RenderCommandList commandList;

  commandList.beginRenderPass((VkRenderPass)0x34343434, (VkFramebuffer)0x232322,
                              {0.5, 1.0}, {800, 600}, {});
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BeginRenderPass);
}

TEST(RenderCommandListTests, RecordEndRenderPass) {
  liquid::RenderCommandList commandList;

  commandList.endRenderPass();
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::EndRenderPass);
}

TEST(RenderCommandListTests, RecordBindPipeline) {
  liquid::RenderCommandList commandList;

  commandList.bindPipeline((VkPipeline)0x34343434,
                           VK_PIPELINE_BIND_POINT_GRAPHICS);
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BindPipeline);
}

TEST(RenderCommandListTests, RecordBindDescriptorSets) {
  liquid::RenderCommandList commandList;

  commandList.bindDescriptorSets((VkPipelineLayout)0x2323232,
                                 VK_PIPELINE_BIND_POINT_GRAPHICS, 0, {}, {});
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BindDescriptorSets);
}

TEST(RenderCommandListDeathTest, BindVertexBufferFailsIfBufferIsNotVertex) {
  liquid::RenderCommandList commandList;
  auto *buffer = new TestBuffer(liquid::HardwareBuffer::INDEX, 0);
  EXPECT_DEATH(commandList.bindVertexBuffer(buffer), ".*");
  delete buffer;
}

TEST(RenderCommandListTests, RecordBindVertexBuffer) {
  liquid::RenderCommandList commandList;

  auto *buffer = new TestBuffer(liquid::HardwareBuffer::VERTEX, 0);

  commandList.bindVertexBuffer(buffer);
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BindVertexBuffer);

  delete buffer;
}

TEST(RenderCommandListDeathTest, BindIndexBufferFailsIfBufferIsNotVertex) {
  liquid::RenderCommandList commandList;
  auto *buffer = new TestBuffer(liquid::HardwareBuffer::VERTEX, 0);
  EXPECT_DEATH(commandList.bindIndexBuffer(buffer, VK_INDEX_TYPE_UINT32), ".*");
  delete buffer;
}

TEST(RenderCommandListTests, RecordBindIndexBuffer) {
  liquid::RenderCommandList commandList;

  auto *buffer = new TestBuffer(liquid::HardwareBuffer::INDEX, 0);

  commandList.bindIndexBuffer(buffer, VK_INDEX_TYPE_UINT32);
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::BindIndexBuffer);

  delete buffer;
}

TEST(RenderCommandListTests, RecordPushConstants) {
  liquid::RenderCommandList commandList;

  commandList.pushConstants((VkPipelineLayout)0x112121,
                            VK_SHADER_STAGE_VERTEX_BIT, 0, 0, nullptr);
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::PushConstants);
}

TEST(RenderCommandListTests, SetViewport) {
  liquid::RenderCommandList commandList;

  commandList.setViewport({0, 0}, {800, 600}, {0.0, 1.0});
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::SetViewport);
}

TEST(RenderCommandListTests, SetScissor) {
  liquid::RenderCommandList commandList;

  commandList.setScissor({0, 0}, {800, 600});
  EXPECT_EQ(commandList.getRecordedCommands().at(0)->type,
            liquid::RenderCommandType::SetScissor);
}
