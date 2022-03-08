#include "liquid/core/Base.h"
#include "liquid/renderer/render-graph/RenderGraphRegistry.h"

#include <gtest/gtest.h>

#include "../mocks/TestPipeline.h"
#include "../mocks/TestRenderPass.h"

class RenderGraphRegistryTest : public ::testing::Test {
public:
  liquid::RenderGraphRegistry registry;
};

TEST_F(RenderGraphRegistryTest, AddsTextureToRegistry) {
  registry.addTexture(1, 2);

  EXPECT_TRUE(registry.hasTexture(1));
  EXPECT_EQ(registry.getTexture(1), 2);
}

TEST_F(RenderGraphRegistryTest, UpdatesExistingTextureInRegistry) {
  registry.addTexture(1, 2);
  registry.addTexture(1, 3);

  EXPECT_TRUE(registry.hasTexture(1));
  EXPECT_EQ(registry.getTexture(1), 3);
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
