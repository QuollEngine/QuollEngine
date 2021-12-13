#include "core/Base.h"
#include "renderer/render-graph/RenderGraph.h"

#include <gtest/gtest.h>

class RenderGraphBuilderTest : public ::testing::Test {
public:
  liquid::RenderGraph graph;
};

class RenderGraphBuilderDeathTest : public RenderGraphBuilderTest {};

struct NoncePass : public liquid::RenderGraphPassInterface {
  NoncePass(liquid::GraphResourceId renderPass)
      : RenderGraphPassInterface(renderPass) {}

  void build(liquid::RenderGraphBuilder &&builder) {}
  void execute(liquid::RenderCommandList &commandList) {}
  const liquid::String &getName() const { return name; }

  liquid::String name = "";
};

TEST_F(RenderGraphBuilderTest, CreatesAttachmentInGraphAndAddsItAsPassOutput) {
  auto pass1 = std::make_shared<NoncePass>(1);

  liquid::RenderGraphBuilder builder(graph, pass1.get());
  liquid::RenderPassAttachment color{liquid::AttachmentType::Color,
                                     {},
                                     liquid::AttachmentLoadOp::Load,
                                     liquid::AttachmentStoreOp::Store,
                                     glm::vec4{1.0f, 0.0f, 2.0f, 0.5f}};
  auto resourceId = builder.write("Test color", color);

  EXPECT_EQ(resourceId, graph.getResourceId("Test color"));

  const auto &stored = graph.getAttachment(resourceId);
  EXPECT_TRUE(std::get<glm::vec4>(stored.clearValue) ==
              glm::vec4(1.0f, 0.0f, 2.0f, 0.5f));
  EXPECT_EQ(pass1->getInputs().size(), 0);
  EXPECT_EQ(pass1->getOutputs().size(), 1);
  EXPECT_EQ(pass1->getOutputs().at(0), resourceId);

  EXPECT_FALSE(pass1->isSwapchainRelative());
}

TEST_F(RenderGraphBuilderTest,
       UpdatesAttachmentInGraphIfIdExistsAndAddsItAsPassOutput) {
  auto pass1 = std::make_shared<NoncePass>(1);
  auto pass2 = std::make_shared<NoncePass>(2);

  auto prevResourceId = std::numeric_limits<liquid::GraphResourceId>::max();

  {
    liquid::RenderGraphBuilder builder(graph, pass2.get());
    prevResourceId = builder.read("Test color");
  }

  {
    liquid::RenderGraphBuilder builder(graph, pass1.get());
    liquid::RenderPassAttachment color{liquid::AttachmentType::Color,
                                       {},
                                       liquid::AttachmentLoadOp::Load,
                                       liquid::AttachmentStoreOp::Store,
                                       glm::vec4{1.0f, 0.0f, 2.0f, 0.5f}};

    auto resourceId = builder.write("Test color", color);

    EXPECT_EQ(resourceId, graph.getResourceId("Test color"));
    EXPECT_EQ(resourceId, pass2->getInputs().at(0));
    EXPECT_EQ(resourceId, prevResourceId);

    const auto &stored = graph.getAttachment(resourceId);
    EXPECT_TRUE(std::get<glm::vec4>(stored.clearValue) ==
                glm::vec4(1.0f, 0.0f, 2.0f, 0.5f));
    EXPECT_EQ(pass1->getInputs().size(), 0);
    EXPECT_EQ(pass1->getOutputs().size(), 1);
    EXPECT_EQ(pass1->getOutputs().at(0), resourceId);

    EXPECT_FALSE(pass1->isSwapchainRelative());
    EXPECT_FALSE(pass2->isSwapchainRelative());
  }
}

TEST_F(RenderGraphBuilderDeathTest,
       FailsToSetAttachmentIfAttachmentAlreadyExists) {
  auto pass1 = std::make_shared<NoncePass>(1);

  liquid::RenderGraphBuilder builder(graph, pass1.get());
  liquid::RenderPassAttachment color{liquid::AttachmentType::Color,
                                     {},
                                     liquid::AttachmentLoadOp::Load,
                                     liquid::AttachmentStoreOp::Store,
                                     glm::vec4{1.0f, 0.0f, 2.0f, 0.5f}};
  auto resourceId = builder.write("Test color", color);
  EXPECT_DEATH(builder.write("Test color", {}), ".*");
}

TEST_F(RenderGraphBuilderTest,
       CreatesResourceIdWithoutReferenceIfResourceDoesNotExist) {
  auto pass1 = std::make_shared<NoncePass>(1);

  liquid::RenderGraphBuilder builder(graph, pass1.get());

  auto resourceId = builder.read("Something");
  EXPECT_TRUE(graph.hasResourceId("Something"));
  EXPECT_EQ(pass1->getInputs().at(0), resourceId);
  EXPECT_FALSE(pass1->isSwapchainRelative());
  EXPECT_FALSE(graph.hasAttachment(resourceId));
}

TEST_F(RenderGraphBuilderTest, SetsAttachmentIdAsInputForPass) {
  auto pass1 = std::make_shared<NoncePass>(1);
  auto pass2 = std::make_shared<NoncePass>(2);

  {
    liquid::RenderGraphBuilder builder(graph, pass1.get());
    liquid::RenderPassAttachment color{liquid::AttachmentType::Color,
                                       {},
                                       liquid::AttachmentLoadOp::Load,
                                       liquid::AttachmentStoreOp::Store,
                                       glm::vec4{1.0f, 0.0f, 2.0f, 0.5f}};
    builder.write("Test color", color);
  }

  {
    liquid::RenderGraphBuilder builder(graph, pass2.get());
    auto resourceId = builder.read("Test color");

    EXPECT_EQ(pass2->getInputs().size(), 1);
    EXPECT_EQ(pass2->getOutputs().size(), 0);
    EXPECT_FALSE(pass2->isSwapchainRelative());

    EXPECT_EQ(pass2->getInputs().at(0), pass1->getOutputs().at(0));
    EXPECT_EQ(pass2->getInputs().at(0), resourceId);
  }
}

TEST_F(RenderGraphBuilderDeathTest,
       FailsToSetSwapchainAttachmentIfAttachmentAlreadyExists) {
  auto pass1 = std::make_shared<NoncePass>(1);

  liquid::RenderGraphBuilder builder(graph, pass1.get());
  auto resourceId = builder.writeSwapchain("Test color", {});
  EXPECT_DEATH(builder.writeSwapchain("Test color", {}), ".*");
}

TEST_F(RenderGraphBuilderTest,
       CreatesSwapchainAttachmentInGraphAndAddsItAsPassOutput) {
  auto pass1 = std::make_shared<NoncePass>(1);

  liquid::RenderGraphBuilder builder(graph, pass1.get());
  liquid::RenderPassSwapchainAttachment color{
      liquid::AttachmentType::Color, liquid::AttachmentLoadOp::Load,
      liquid::AttachmentStoreOp::Store, glm::vec4{1.0f, 0.0f, 2.0f, 0.5f}};
  auto resourceId = builder.writeSwapchain("Test color", color);
  const auto &stored = graph.getSwapchainAttachment(resourceId);

  EXPECT_EQ(resourceId, graph.getResourceId("Test color"));
  EXPECT_FALSE(graph.hasAttachment(resourceId));
  EXPECT_TRUE(std::get<glm::vec4>(stored.clearValue) ==
              glm::vec4(1.0f, 0.0f, 2.0f, 0.5f));
  EXPECT_EQ(pass1->getInputs().size(), 0);
  EXPECT_EQ(pass1->getOutputs().size(), 1);
  EXPECT_EQ(pass1->getOutputs().at(0), resourceId);

  EXPECT_TRUE(pass1->isSwapchainRelative());
}

TEST_F(RenderGraphBuilderTest, CreatesPipelineResource) {
  auto pass1 = std::make_shared<NoncePass>(1);

  liquid::PipelineDescriptor descriptor{};
  descriptor.inputAssembly.primitiveTopology =
      liquid::PrimitiveTopology::PatchList;

  liquid::RenderGraphBuilder builder(graph, pass1.get());
  auto resourceId = builder.create(descriptor);

  EXPECT_EQ(graph.getPipeline(resourceId).inputAssembly.primitiveTopology,
            liquid::PrimitiveTopology::PatchList);
}
