#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid/renderer/RenderGraphSyncDependency.h"

class RenderGraphSyncDependencyTextureReadTest : public ::testing::Test {};

TEST_F(RenderGraphSyncDependencyTextureReadTest,
       ReturnsComputeShaderWhenPassIsCompute) {
  auto dependency = liquid::RenderGraphSyncDependency::getTextureRead(
      liquid::RenderGraphPassType::Compute);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderRead);
  EXPECT_EQ(dependency.layout, liquid::rhi::ImageLayout::ShaderReadOnlyOptimal);
}

TEST_F(RenderGraphSyncDependencyTextureReadTest,
       ReturnsFragmentShaderDependencyWhenPassIsGraphics) {
  auto dependency = liquid::RenderGraphSyncDependency::getTextureRead(
      liquid::RenderGraphPassType::Graphics);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderRead);
  EXPECT_EQ(dependency.layout, liquid::rhi::ImageLayout::ShaderReadOnlyOptimal);
}

class RenderGraphSyncDependencyTextureWriteTest : public ::testing::Test {};

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsColorAttachmentWriteWhenPassIsGraphicsAndAttachmentIsColor) {
  auto dependency = liquid::RenderGraphSyncDependency::getTextureWrite(
      liquid::RenderGraphPassType::Graphics, liquid::AttachmentType::Color);

  EXPECT_EQ(dependency.stage,
            liquid::rhi::PipelineStage::ColorAttachmentOutput);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ColorAttachmentWrite);
  EXPECT_EQ(dependency.layout,
            liquid::rhi::ImageLayout::ColorAttachmentOptimal);
}

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsColorAttachmentWriteWhenPassIsGraphicsAndAttachmentIsResolve) {
  auto dependency = liquid::RenderGraphSyncDependency::getTextureWrite(
      liquid::RenderGraphPassType::Graphics, liquid::AttachmentType::Resolve);

  EXPECT_EQ(dependency.stage,
            liquid::rhi::PipelineStage::ColorAttachmentOutput);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ColorAttachmentWrite);
  EXPECT_EQ(dependency.layout,
            liquid::rhi::ImageLayout::ColorAttachmentOptimal);
}

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsDepthAttachmentWriteWhenPassIsGraphicsAndAttachmentIsDepth) {
  auto dependency = liquid::RenderGraphSyncDependency::getTextureWrite(
      liquid::RenderGraphPassType::Graphics, liquid::AttachmentType::Depth);

  EXPECT_EQ(dependency.stage,
            liquid::rhi::PipelineStage::EarlyFragmentTests |
                liquid::rhi::PipelineStage::LateFragmentTests);
  EXPECT_EQ(dependency.access,
            liquid::rhi::Access::DepthStencilAttachmentWrite);
  EXPECT_EQ(dependency.layout,
            liquid::rhi::ImageLayout::DepthStencilAttachmentOptimal);
}

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsComputeShaderWhenPassIsComputeAndAttachmentIsColor) {
  auto dependency = liquid::RenderGraphSyncDependency::getTextureWrite(
      liquid::RenderGraphPassType::Compute, liquid::AttachmentType::Color);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderWrite);
  EXPECT_EQ(dependency.layout, liquid::rhi::ImageLayout::General);
}

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsComputeShaderWhenPassIsComputeAndAttachmentIsResolve) {
  auto dependency = liquid::RenderGraphSyncDependency::getTextureWrite(
      liquid::RenderGraphPassType::Compute, liquid::AttachmentType::Resolve);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderWrite);
  EXPECT_EQ(dependency.layout, liquid::rhi::ImageLayout::General);
}

TEST_F(RenderGraphSyncDependencyTextureWriteTest,
       ReturnsComputeShaderWhenPassIsComputeAndAttachmentIsDepth) {
  auto dependency = liquid::RenderGraphSyncDependency::getTextureWrite(
      liquid::RenderGraphPassType::Compute, liquid::AttachmentType::Depth);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderWrite);
  EXPECT_EQ(dependency.layout, liquid::rhi::ImageLayout::General);
}

class RenderGraphSyncDependencyBufferReadTest : public ::testing::Test {};

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsComputeShaderWhenPassIsCompute) {
  auto dependency = liquid::RenderGraphSyncDependency::getBufferRead(
      liquid::RenderGraphPassType::Compute, liquid::rhi::BufferUsage::Indirect);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsVertexReadWhenPassIsGraphicsAndUsageIncludesVertex) {
  auto dependency = liquid::RenderGraphSyncDependency::getBufferRead(
      liquid::RenderGraphPassType::Graphics, liquid::rhi::BufferUsage::Vertex);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::VertexInput);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::VertexAttributeRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsIndexReadWhenPassIsGraphicsAndUsageIncludesIndex) {
  auto dependency = liquid::RenderGraphSyncDependency::getBufferRead(
      liquid::RenderGraphPassType::Graphics, liquid::rhi::BufferUsage::Index);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::VertexInput);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::IndexRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsIndirectReadWhenPassIsGraphicsAndUsageIncludesIndirect) {
  auto dependency = liquid::RenderGraphSyncDependency::getBufferRead(
      liquid::RenderGraphPassType::Graphics,
      liquid::rhi::BufferUsage::Indirect);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::DrawIndirect);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::IndirectCommandRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsFragmentShaderReadWhenBufferUsageIncludesUniform) {
  auto dependency = liquid::RenderGraphSyncDependency::getBufferRead(
      liquid::RenderGraphPassType::Graphics, liquid::rhi::BufferUsage::Uniform);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsFragmentShaderReadWhenBufferUsageIncludesStorage) {
  auto dependency = liquid::RenderGraphSyncDependency::getBufferRead(
      liquid::RenderGraphPassType::Graphics, liquid::rhi::BufferUsage::Storage);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderRead);
}

TEST_F(RenderGraphSyncDependencyBufferReadTest,
       ReturnsMultipleStagesForMultipleUsage) {
  auto dependency = liquid::RenderGraphSyncDependency::getBufferRead(
      liquid::RenderGraphPassType::Graphics,
      liquid::rhi::BufferUsage::Storage | liquid::rhi::BufferUsage::Vertex);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::FragmentShader |
                                  liquid::rhi::PipelineStage::VertexInput);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderRead |
                                   liquid::rhi::Access::VertexAttributeRead);
}

class RenderGraphSyncDependencyBufferWriteTest : public ::testing::Test {};

TEST_F(RenderGraphSyncDependencyBufferWriteTest,
       ReturnsComputeShaderWhenPassIsCompute) {
  auto dependency = liquid::RenderGraphSyncDependency::getBufferWrite(
      liquid::RenderGraphPassType::Compute);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderWrite);
}

TEST_F(RenderGraphSyncDependencyBufferWriteTest,
       ReturnsFragmentShaderWhenPassIsGraphics) {
  auto dependency = liquid::RenderGraphSyncDependency::getBufferWrite(
      liquid::RenderGraphPassType::Graphics);

  EXPECT_EQ(dependency.stage, liquid::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(dependency.access, liquid::rhi::Access::ShaderWrite);
}
