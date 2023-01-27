#include "liquid/core/Base.h"
#include "liquid/renderer/RenderGraphPass.h"

#include "liquid-tests/Testing.h"

class RenderGraphPassTest : public ::testing::Test {
public:
  RenderGraphPassTest()
      : graphicsPass("Graphics", liquid::RenderGraphPassType::Graphics),
        computePass("Compute", liquid::RenderGraphPassType::Compute) {}

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
  liquid::rhi::TextureHandle handle{2};

  graphicsPass.write(handle, glm::vec4());
  EXPECT_EQ(graphicsPass.getTextureOutputs().size(), 1);
  EXPECT_EQ(graphicsPass.getTextureOutputs().at(0).texture, handle);
}

TEST_F(RenderGraphPassTest, AddsClearValueToAttachmentDataOnWrite) {
  liquid::rhi::TextureHandle handle{2};

  graphicsPass.write(handle, glm::vec4(2.0f));
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
  liquid::rhi::TextureHandle handle{2};

  graphicsPass.read(handle);
  EXPECT_EQ(graphicsPass.getAttachments().size(), 0);
  EXPECT_EQ(graphicsPass.getTextureOutputs().size(), 0);
  EXPECT_EQ(graphicsPass.getTextureInputs().size(), 1);
  EXPECT_EQ(graphicsPass.getTextureInputs().at(0).texture, handle);
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithVertexOnGraphicsPass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(graphicsPass.write(handle, liquid::rhi::BufferType::Vertex),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithVertexOnComputePass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.write(handle, liquid::rhi::BufferType::Vertex),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndexOnGraphicsPass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(graphicsPass.write(handle, liquid::rhi::BufferType::Index),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndexOnComputePass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.write(handle, liquid::rhi::BufferType::Index), ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndirectOnGraphicsPass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(graphicsPass.write(handle, liquid::rhi::BufferType::Indirect),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsWritingBufferWithIndirectOnComputePass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.write(handle, liquid::rhi::BufferType::Indirect),
               ".*");
}

TEST_F(RenderGraphPassTest, FailsReadingBufferWithVertexOnComputePass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.read(handle, liquid::rhi::BufferType::Vertex), ".*");
}

TEST_F(RenderGraphPassTest, FailsReadingBufferWithIndexOnComputePass) {
  liquid::rhi::BufferHandle handle{2};

  EXPECT_DEATH(computePass.read(handle, liquid::rhi::BufferType::Index), ".*");
}

TEST_F(RenderGraphPassTest, AddsBufferHandleToOutputOnWrite) {
  liquid::rhi::BufferHandle handle{2};

  graphicsPass.write(handle, liquid::rhi::BufferType::Storage);
  EXPECT_EQ(graphicsPass.getBufferOutputs().size(), 1);
  EXPECT_EQ(graphicsPass.getBufferOutputs().at(0).buffer, handle);
  EXPECT_EQ(graphicsPass.getBufferOutputs().at(0).type,
            liquid::rhi::BufferType::Storage);
}

TEST_F(RenderGraphPassTest, AddsBufferHandleToInputOnRead) {
  liquid::rhi::BufferHandle handle{2};

  computePass.read(handle, liquid::rhi::BufferType::Storage);
  EXPECT_EQ(computePass.getBufferOutputs().size(), 0);
  EXPECT_EQ(computePass.getBufferInputs().size(), 1);
  EXPECT_EQ(computePass.getBufferInputs().at(0).buffer, handle);
  EXPECT_EQ(computePass.getBufferInputs().at(0).type,
            liquid::rhi::BufferType::Storage);
}
