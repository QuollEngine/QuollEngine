#include "quoll/core/Base.h"
#include "quoll/renderer/RenderGraph.h"

#include "quoll-tests/Testing.h"

class RenderGraphPassTest : public ::testing::Test {
public:
  RenderGraphPassTest()
      : graphicsPass("Graphics", quoll::RenderGraphPassType::Graphics),
        computePass("Compute", quoll::RenderGraphPassType::Compute),
        graph("Test graph") {}

  quoll::RenderGraph graph;
  quoll::RenderGraphPass graphicsPass;
  quoll::RenderGraphPass computePass;
};

TEST_F(RenderGraphPassTest, SetsNameAndTypeOnConstruct) {
  EXPECT_EQ(graphicsPass.getName(), "Graphics");
  EXPECT_EQ(graphicsPass.getType(), quoll::RenderGraphPassType::Graphics);

  EXPECT_EQ(computePass.getName(), "Compute");
  EXPECT_EQ(computePass.getType(), quoll::RenderGraphPassType::Compute);
}

TEST_F(RenderGraphPassTest, AddsTextureHandleToOutputOnWrite) {
  auto handle = graph.create(quoll::rhi::TextureDescription{});

  graphicsPass.write(handle, quoll::AttachmentType::Color, glm::vec4());
  EXPECT_EQ(graphicsPass.getTextureOutputs().size(), 1);
  EXPECT_EQ(graphicsPass.getTextureOutputs().at(0).texture, handle);
}

TEST_F(RenderGraphPassTest, AddsClearValueToAttachmentDataOnWrite) {
  auto handle = graph.create(quoll::rhi::TextureDescription{});

  graphicsPass.write(handle, quoll::AttachmentType::Color, glm::vec4(2.0f));
  EXPECT_EQ(graphicsPass.getAttachments().size(), 1);
  EXPECT_EQ(std::get<glm::vec4>(graphicsPass.getAttachments().at(0).clearValue),
            glm::vec4(2.0f));

  // Defaults
  EXPECT_EQ(graphicsPass.getAttachments().at(0).loadOp,
            quoll::rhi::AttachmentLoadOp::DontCare);
  EXPECT_EQ(graphicsPass.getAttachments().at(0).storeOp,
            quoll::rhi::AttachmentStoreOp::DontCare);
}

TEST_F(RenderGraphPassTest, AddsTextureHandleToInputOnRead) {
  auto handle = graph.create(quoll::rhi::TextureDescription{});

  graphicsPass.read(handle);
  EXPECT_EQ(graphicsPass.getAttachments().size(), 0);
  EXPECT_EQ(graphicsPass.getTextureOutputs().size(), 0);
  EXPECT_EQ(graphicsPass.getTextureInputs().size(), 1);
  EXPECT_EQ(graphicsPass.getTextureInputs().at(0).texture, handle);
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithVertexOnGraphicsPass) {
  quoll::rhi::BufferHandle handle{2};

  EXPECT_DEATH(graphicsPass.write(handle, quoll::rhi::BufferUsage::Vertex),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithVertexOnComputePass) {
  quoll::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.write(handle, quoll::rhi::BufferUsage::Vertex),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndexOnGraphicsPass) {
  quoll::rhi::BufferHandle handle{2};

  EXPECT_DEATH(graphicsPass.write(handle, quoll::rhi::BufferUsage::Index),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndexOnComputePass) {
  quoll::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.write(handle, quoll::rhi::BufferUsage::Index), ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndirectOnGraphicsPass) {
  quoll::rhi::BufferHandle handle{2};

  EXPECT_DEATH(graphicsPass.write(handle, quoll::rhi::BufferUsage::Indirect),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndirectOnComputePass) {
  quoll::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.write(handle, quoll::rhi::BufferUsage::Indirect),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsReadingBufferWithVertexOnComputePass) {
  quoll::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.read(handle, quoll::rhi::BufferUsage::Vertex), ".*");
}

TEST_F(RenderGraphPassTest, FailsReadingBufferWithIndexOnComputePass) {
  quoll::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.read(handle, quoll::rhi::BufferUsage::Index), ".*");
}

TEST_F(RenderGraphPassTest, AddsBufferHandleToOutputOnWrite) {
  quoll::rhi::BufferHandle handle{2};

  graphicsPass.write(handle, quoll::rhi::BufferUsage::Storage);
  EXPECT_EQ(graphicsPass.getBufferOutputs().size(), 1);
  EXPECT_EQ(graphicsPass.getBufferOutputs().at(0).buffer, handle);
  EXPECT_EQ(graphicsPass.getBufferOutputs().at(0).usage,
            quoll::rhi::BufferUsage::Storage);
}

TEST_F(RenderGraphPassTest, AddsBufferHandleToInputOnRead) {
  quoll::rhi::BufferHandle handle{2};

  computePass.read(handle, quoll::rhi::BufferUsage::Storage);
  EXPECT_EQ(computePass.getBufferOutputs().size(), 0);
  EXPECT_EQ(computePass.getBufferInputs().size(), 1);
  EXPECT_EQ(computePass.getBufferInputs().at(0).buffer, handle);
  EXPECT_EQ(computePass.getBufferInputs().at(0).usage,
            quoll::rhi::BufferUsage::Storage);
}
