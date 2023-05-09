#include "liquid/core/Base.h"
#include "liquid/renderer/RenderGraph.h"

#include "liquid-tests/Testing.h"

class RenderGraphPassTest : public ::testing::Test {
public:
  RenderGraphPassTest()
      : graphicsPass("Graphics", liquid::RenderGraphPassType::Graphics),
        computePass("Compute", liquid::RenderGraphPassType::Compute),
        graph("Test graph") {}

  liquid::RenderGraph graph;
  liquid::RenderGraphPass graphicsPass;
  liquid::RenderGraphPass computePass;
};

TEST_F(RenderGraphPassTest, SetsNameAndTypeOnConstruct) {
  EXPECT_EQ(graphicsPass.getName(), "Graphics");
  EXPECT_EQ(graphicsPass.getType(), liquid::RenderGraphPassType::Graphics);

  EXPECT_EQ(computePass.getName(), "Compute");
  EXPECT_EQ(computePass.getType(), liquid::RenderGraphPassType::Compute);
}

TEST_F(RenderGraphPassTest, AddsTextureHandleToOutputOnWrite) {
  auto handle =
      graph.create(liquid::rhi::TextureDescription{}, [](auto, auto &) {});

  graphicsPass.write(handle, liquid::AttachmentType::Color, glm::vec4());
  EXPECT_EQ(graphicsPass.getTextureOutputs().size(), 1);
  EXPECT_EQ(graphicsPass.getTextureOutputs().at(0).texture, handle);
}

TEST_F(RenderGraphPassTest, AddsClearValueToAttachmentDataOnWrite) {
  auto handle =
      graph.create(liquid::rhi::TextureDescription{}, [](auto, auto &) {});

  graphicsPass.write(handle, liquid::AttachmentType::Color, glm::vec4(2.0f));
  EXPECT_EQ(graphicsPass.getAttachments().size(), 1);
  EXPECT_EQ(std::get<glm::vec4>(graphicsPass.getAttachments().at(0).clearValue),
            glm::vec4(2.0f));

  // Defaults
  EXPECT_EQ(graphicsPass.getAttachments().at(0).loadOp,
            liquid::rhi::AttachmentLoadOp::DontCare);
  EXPECT_EQ(graphicsPass.getAttachments().at(0).storeOp,
            liquid::rhi::AttachmentStoreOp::DontCare);
}

TEST_F(RenderGraphPassTest, AddsTextureHandleToInputOnRead) {
  auto handle =
      graph.create(liquid::rhi::TextureDescription{}, [](auto, auto &) {});

  graphicsPass.read(handle);
  EXPECT_EQ(graphicsPass.getAttachments().size(), 0);
  EXPECT_EQ(graphicsPass.getTextureOutputs().size(), 0);
  EXPECT_EQ(graphicsPass.getTextureInputs().size(), 1);
  EXPECT_EQ(graphicsPass.getTextureInputs().at(0).texture, handle);
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithVertexOnGraphicsPass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(graphicsPass.write(handle, liquid::rhi::BufferUsage::Vertex),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithVertexOnComputePass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.write(handle, liquid::rhi::BufferUsage::Vertex),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndexOnGraphicsPass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(graphicsPass.write(handle, liquid::rhi::BufferUsage::Index),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndexOnComputePass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.write(handle, liquid::rhi::BufferUsage::Index),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndirectOnGraphicsPass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(graphicsPass.write(handle, liquid::rhi::BufferUsage::Indirect),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndirectOnComputePass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.write(handle, liquid::rhi::BufferUsage::Indirect),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsReadingBufferWithVertexOnComputePass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.read(handle, liquid::rhi::BufferUsage::Vertex),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsReadingBufferWithIndexOnComputePass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.read(handle, liquid::rhi::BufferUsage::Index), ".*");
}

TEST_F(RenderGraphPassTest, AddsBufferHandleToOutputOnWrite) {
  liquid::rhi::BufferHandle handle{2};

  graphicsPass.write(handle, liquid::rhi::BufferUsage::Storage);
  EXPECT_EQ(graphicsPass.getBufferOutputs().size(), 1);
  EXPECT_EQ(graphicsPass.getBufferOutputs().at(0).buffer, handle);
  EXPECT_EQ(graphicsPass.getBufferOutputs().at(0).usage,
            liquid::rhi::BufferUsage::Storage);
}

TEST_F(RenderGraphPassTest, AddsBufferHandleToInputOnRead) {
  liquid::rhi::BufferHandle handle{2};

  computePass.read(handle, liquid::rhi::BufferUsage::Storage);
  EXPECT_EQ(computePass.getBufferOutputs().size(), 0);
  EXPECT_EQ(computePass.getBufferInputs().size(), 1);
  EXPECT_EQ(computePass.getBufferInputs().at(0).buffer, handle);
  EXPECT_EQ(computePass.getBufferInputs().at(0).usage,
            liquid::rhi::BufferUsage::Storage);
}
