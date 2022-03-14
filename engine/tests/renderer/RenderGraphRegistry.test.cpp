#include "liquid/core/Base.h"
#include "liquid/renderer/render-graph/RenderGraphRegistry.h"

#include <gtest/gtest.h>

class RenderGraphRegistryTest : public ::testing::Test {
public:
  liquid::RenderGraphRegistry registry;
};

TEST_F(RenderGraphRegistryTest, AddsTextureToRegistry) {
  registry.addTexture(1, liquid::rhi::TextureHandle(2));

  EXPECT_TRUE(registry.hasTexture(1));
  EXPECT_EQ(registry.getTexture(1), liquid::rhi::TextureHandle(2));
}

TEST_F(RenderGraphRegistryTest, UpdatesExistingTextureInRegistry) {
  registry.addTexture(1, liquid::rhi::TextureHandle(2));
  registry.addTexture(1, liquid::rhi::TextureHandle(3));

  EXPECT_TRUE(registry.hasTexture(1));
  EXPECT_EQ(registry.getTexture(1), liquid::rhi::TextureHandle(3));
}

TEST_F(RenderGraphRegistryTest, AddsRenderPassToRegistry) {
  registry.addRenderPass(
      1, liquid::RenderGraphPassResult(liquid::rhi::RenderPassHandle(2), {}, {},
                                       100, 200, 1));

  EXPECT_TRUE(registry.hasRenderPass(1));
  EXPECT_EQ(registry.getRenderPass(1).getRenderPass(),
            liquid::rhi::RenderPassHandle(2));
}

TEST_F(RenderGraphRegistryTest, UpdatesExistingRenderPassInRegistry) {
  registry.addRenderPass(
      1, liquid::RenderGraphPassResult(liquid::rhi::RenderPassHandle(2), {}, {},
                                       100, 200, 1));
  registry.addRenderPass(
      1, liquid::RenderGraphPassResult(liquid::rhi::RenderPassHandle(3), {}, {},
                                       100, 200, 1));

  EXPECT_TRUE(registry.hasRenderPass(1));
  EXPECT_EQ(registry.getRenderPass(1).getRenderPass(),
            liquid::rhi::RenderPassHandle(3));
}

TEST_F(RenderGraphRegistryTest, AddsPipelineToRegistry) {
  registry.addPipeline(1, liquid::rhi::PipelineHandle(2));

  EXPECT_TRUE(registry.hasPipeline(1));
  EXPECT_EQ(registry.getPipeline(1), liquid::rhi::PipelineHandle(2));
}

TEST_F(RenderGraphRegistryTest, UpdatesExistingPipelineInRegistry) {
  registry.addPipeline(1, liquid::rhi::PipelineHandle(2));
  registry.addPipeline(1, liquid::rhi::PipelineHandle(3));

  EXPECT_TRUE(registry.hasPipeline(1));
  EXPECT_EQ(registry.getPipeline(1), liquid::rhi::PipelineHandle(3));
}
