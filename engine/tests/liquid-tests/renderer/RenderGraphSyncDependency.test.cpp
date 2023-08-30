#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid/renderer/RenderGraphSyncDependency.h"

class RenderGraphSyncDependencyTextureReadTest : public ::testing::Test {};

TEST_F(RenderGraphSyncDependencyTextureReadTest,
       ReturnsComputeShaderWhenPassIsCompute) {
  auto dependency = quoll::RenderGraphSyncDependency::getTextureRead(
      quoll::RenderGraphPassType::Compute);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderRead);
  EXPECT_EQ(dependency.layout, quoll::rhi::ImageLayout::ShaderReadOnlyOptimal);
}

TEST_F(RenderGraphSyncDependencyTextureReadTest,
       ReturnsFragmentShaderDependencyWhenPassIsGraphics) {
  auto dependency = quoll::RenderGraphSyncDependency::getTextureRead(
      quoll::RenderGraphPassType::Graphics);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderRead);
  EXPECT_EQ(dependency.layout, quoll::rhi::ImageLayout::ShaderReadOnlyOptimal);
}

class RenderGraphSyncDependencyTextureWriteTest : public ::testing::Test {};

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsColorAttachmentWriteWhenPassIsGraphicsAndAttachmentIsColor) {
  auto dependency = quoll::RenderGraphSyncDependency::getTextureWrite(
      quoll::RenderGraphPassType::Graphics, quoll::AttachmentType::Color);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::ColorAttachmentOutput);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ColorAttachmentWrite);
  EXPECT_EQ(dependency.layout, quoll::rhi::ImageLayout::ColorAttachmentOptimal);
}

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsColorAttachmentWriteWhenPassIsGraphicsAndAttachmentIsResolve) {
  auto dependency = quoll::RenderGraphSyncDependency::getTextureWrite(
      quoll::RenderGraphPassType::Graphics, quoll::AttachmentType::Resolve);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::ColorAttachmentOutput);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ColorAttachmentWrite);
  EXPECT_EQ(dependency.layout, quoll::rhi::ImageLayout::ColorAttachmentOptimal);
}

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsDepthAttachmentWriteWhenPassIsGraphicsAndAttachmentIsDepth) {
  auto dependency = quoll::RenderGraphSyncDependency::getTextureWrite(
      quoll::RenderGraphPassType::Graphics, quoll::AttachmentType::Depth);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::EarlyFragmentTests |
                                  quoll::rhi::PipelineStage::LateFragmentTests);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::DepthStencilAttachmentWrite);
  EXPECT_EQ(dependency.layout,
            quoll::rhi::ImageLayout::DepthStencilAttachmentOptimal);
}

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsComputeShaderWhenPassIsComputeAndAttachmentIsColor) {
  auto dependency = quoll::RenderGraphSyncDependency::getTextureWrite(
      quoll::RenderGraphPassType::Compute, quoll::AttachmentType::Color);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderWrite);
  EXPECT_EQ(dependency.layout, quoll::rhi::ImageLayout::General);
}

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsComputeShaderWhenPassIsComputeAndAttachmentIsResolve) {
  auto dependency = quoll::RenderGraphSyncDependency::getTextureWrite(
      quoll::RenderGraphPassType::Compute, quoll::AttachmentType::Resolve);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderWrite);
  EXPECT_EQ(dependency.layout, quoll::rhi::ImageLayout::General);
}

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsComputeShaderWhenPassIsComputeAndAttachmentIsDepth) {
  auto dependency = quoll::RenderGraphSyncDependency::getTextureWrite(
      quoll::RenderGraphPassType::Compute, quoll::AttachmentType::Depth);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderWrite);
  EXPECT_EQ(dependency.layout, quoll::rhi::ImageLayout::General);
}

class RenderGraphSyncDependencyBufferReadTest : public ::testing::Test {};

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsComputeShaderWhenPassIsCompute) {
  auto dependency = quoll::RenderGraphSyncDependency::getBufferRead(
      quoll::RenderGraphPassType::Compute, quoll::rhi::BufferUsage::Indirect);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsVertexReadWhenPassIsGraphicsAndUsageIncludesVertex) {
  auto dependency = quoll::RenderGraphSyncDependency::getBufferRead(
      quoll::RenderGraphPassType::Graphics, quoll::rhi::BufferUsage::Vertex);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::VertexAttributeInput);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::VertexAttributeRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsIndexReadWhenPassIsGraphicsAndUsageIncludesIndex) {
  auto dependency = quoll::RenderGraphSyncDependency::getBufferRead(
      quoll::RenderGraphPassType::Graphics, quoll::rhi::BufferUsage::Index);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::IndexInput);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::IndexRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsIndirectReadWhenPassIsGraphicsAndUsageIncludesIndirect) {
  auto dependency = quoll::RenderGraphSyncDependency::getBufferRead(
      quoll::RenderGraphPassType::Graphics, quoll::rhi::BufferUsage::Indirect);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::DrawIndirect);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::IndirectCommandRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsFragmentShaderReadWhenBufferUsageIncludesUniform) {
  auto dependency = quoll::RenderGraphSyncDependency::getBufferRead(
      quoll::RenderGraphPassType::Graphics, quoll::rhi::BufferUsage::Uniform);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsFragmentShaderReadWhenBufferUsageIncludesStorage) {
  auto dependency = quoll::RenderGraphSyncDependency::getBufferRead(
      quoll::RenderGraphPassType::Graphics, quoll::rhi::BufferUsage::Storage);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsMultipleStagesForMultipleUsage) {
  auto dependency = quoll::RenderGraphSyncDependency::getBufferRead(
      quoll::RenderGraphPassType::Graphics,
      quoll::rhi::BufferUsage::Storage | quoll::rhi::BufferUsage::Vertex);

  EXPECT_EQ(dependency.stage,
            quoll::rhi::PipelineStage::FragmentShader |
                quoll::rhi::PipelineStage::VertexAttributeInput);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderRead |
                                   quoll::rhi::Access::VertexAttributeRead);
}

class RenderGraphSyncDependencyBufferWriteTest : public ::testing::Test {};

TEST_F(RenderGraphSyncDependencyBufferWriteTest,
       ReturnsComputeShaderWhenPassIsCompute) {
  auto dependency = quoll::RenderGraphSyncDependency::getBufferWrite(
      quoll::RenderGraphPassType::Compute);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderWrite);
}

TEST_F(RenderGraphSyncDependencyBufferWriteTest,
       ReturnsFragmentShaderWhenPassIsGraphics) {
  auto dependency = quoll::RenderGraphSyncDependency::getBufferWrite(
      quoll::RenderGraphPassType::Graphics);

  EXPECT_EQ(dependency.stage, quoll::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(dependency.access, quoll::rhi::Access::ShaderWrite);
}
