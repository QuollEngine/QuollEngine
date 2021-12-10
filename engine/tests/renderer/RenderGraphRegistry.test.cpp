#include "core/Base.h"
#include "renderer/render-graph/RenderGraphRegistry.h"
#include "renderer/Texture.h"

#include <gtest/gtest.h>

#include "../mocks/TestPipeline.h"
#include "../mocks/TestRenderPass.h"

class RenderGraphRegistryTest : public ::testing::Test {
public:
  liquid::RenderGraphRegistry registry;
  liquid::StatsManager statsManager;
};

TEST_F(RenderGraphRegistryTest, AddsTextureToRegistry) {
  registry.addTexture(
      1, std::make_shared<liquid::Texture>(nullptr, 100, statsManager));

  EXPECT_TRUE(registry.hasTexture(1));
  EXPECT_EQ(registry.getTexture(1)->getSize(), 100);
}

TEST_F(RenderGraphRegistryTest, UpdatedExistingTextureInRegistry) {
  registry.addTexture(
      1, std::make_shared<liquid::Texture>(nullptr, 100, statsManager));
  registry.addTexture(
      1, std::make_shared<liquid::Texture>(nullptr, 250, statsManager));

  EXPECT_TRUE(registry.hasTexture(1));
  EXPECT_EQ(registry.getTexture(1)->getSize(), 250);
}
TEST_F(RenderGraphRegistryTest, AddsRenderPassToRegistry) {
  registry.addRenderPass(1, std::make_shared<TestRenderPass>("Pass1"));

  EXPECT_TRUE(registry.hasRenderPass(1));
  EXPECT_EQ(std::dynamic_pointer_cast<TestRenderPass>(registry.getRenderPass(1))
                ->getName(),
            "Pass1");
}

TEST_F(RenderGraphRegistryTest, UpdatesExistingRenderPassInRegistry) {
  registry.addRenderPass(1, std::make_shared<TestRenderPass>("Pass1"));
  registry.addRenderPass(1, std::make_shared<TestRenderPass>("Pass2"));

  EXPECT_TRUE(registry.hasRenderPass(1));
  EXPECT_EQ(std::dynamic_pointer_cast<TestRenderPass>(registry.getRenderPass(1))
                ->getName(),
            "Pass2");
}

TEST_F(RenderGraphRegistryTest, AddsPipelineToRegistry) {
  registry.addPipeline(1, std::make_shared<TestPipeline>("Pipeline1"));

  EXPECT_TRUE(registry.hasPipeline(1));
  EXPECT_EQ(std::dynamic_pointer_cast<TestPipeline>(registry.getPipeline(1))
                ->getName(),
            "Pipeline1");
}

TEST_F(RenderGraphRegistryTest, UpdatesExistingPipelineInRegistry) {
  registry.addPipeline(1, std::make_shared<TestPipeline>("Pipeline1"));
  registry.addPipeline(1, std::make_shared<TestPipeline>("Pipeline2"));

  EXPECT_TRUE(registry.hasPipeline(1));
  EXPECT_EQ(std::dynamic_pointer_cast<TestPipeline>(registry.getPipeline(1))
                ->getName(),
            "Pipeline2");
}
