#include "liquid/core/Base.h"
#include "liquid/renderer/render-graph/RenderGraph.h"

#include <gtest/gtest.h>

class RenderGraphBuilderTest : public ::testing::Test {
public:
  liquid::RenderGraph graph;
};

using RenderGraphBuilderDeathTest = RenderGraphBuilderTest;

struct NoncePass : public liquid::RenderGraphPassBase {
  NoncePass(const liquid::String &name, liquid::GraphResourceId renderPass)
      : RenderGraphPassBase(name, renderPass) {}

  void buildInternal(liquid::RenderGraphBuilder &builder) {}
  void execute(liquid::rhi::RenderCommandList &commandList,
               liquid::RenderGraphRegistry &registry) {}
};

TEST_F(RenderGraphBuilderDeathTest,
       FailsToAddResourceOutputIfResourceDoesNotExist) {
  auto pass = std::make_shared<NoncePass>("nonce", 1);

  liquid::RenderGraphBuilder builder(graph, pass.get());
  EXPECT_DEATH({ builder.write("Test color"); }, ".*");
}

TEST_F(RenderGraphBuilderTest, AddsResourceIdAsPassOutput) {
  auto pass = std::make_shared<NoncePass>("nonce", 1);

  liquid::rhi::TextureHandle handle{2};
  graph.import("Test color", handle, {});

  liquid::RenderGraphBuilder builder(graph, pass.get());
  auto outputId = builder.write("Test color");

  EXPECT_EQ(outputId, graph.getResourceId("Test color"));

  EXPECT_EQ(pass->getInputs().size(), 0);
  EXPECT_EQ(pass->getOutputs().size(), 1);
  EXPECT_EQ(graph.getResourceId("Test color"), outputId);
}

TEST_F(RenderGraphBuilderTest, AddsResourceIdAsPassInput) {
  auto pass = std::make_shared<NoncePass>("nonce1", 1);

  liquid::rhi::TextureHandle resourceId{2};
  graph.import("Test color", resourceId, {});

  liquid::RenderGraphBuilder builder(graph, pass.get());
  auto inputId = builder.read("Test color");

  EXPECT_EQ(pass->getInputs().size(), 1);
  EXPECT_EQ(pass->getOutputs().size(), 0);

  EXPECT_EQ(pass->getInputs().at(0), resourceId);
  EXPECT_EQ(resourceId, inputId);
}

TEST_F(RenderGraphBuilderDeathTest,
       FailsToAddResourceInputIfResourceDoesNotExist) {
  auto pass = std::make_shared<NoncePass>("nonce", 1);

  liquid::RenderGraphBuilder builder(graph, pass.get());
  EXPECT_DEATH({ builder.read("Test color"); }, ".*");
}

TEST_F(RenderGraphBuilderTest, CreatesPipelineResource) {
  auto pass1 = std::make_shared<NoncePass>("nonce", 1);

  liquid::RenderGraphPipelineDescription descriptor{};
  descriptor.inputAssembly.primitiveTopology =
      liquid::PrimitiveTopology::PatchList;

  liquid::RenderGraphBuilder builder(graph, pass1.get());
  auto resourceId = builder.create(descriptor);

  EXPECT_EQ(graph.getPipeline(resourceId).inputAssembly.primitiveTopology,
            liquid::PrimitiveTopology::PatchList);
}
