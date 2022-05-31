#include "liquid/core/Base.h"
#include "liquid/rhi/RenderGraph.h"

#include <gtest/gtest.h>

class RenderGraphTest : public ::testing::Test {
public:
  liquid::rhi::ResourceRegistry resourceRegistry;
  liquid::rhi::RenderGraph graph;
};

class RenderGraphDeathTest : public RenderGraphTest {};

TEST_F(RenderGraphTest, AddsPass) {
  auto &pass = graph.addPass("Test");
  EXPECT_EQ(pass.getName(), "Test");
  EXPECT_EQ(graph.getPasses().at(0).getName(), "Test");
  EXPECT_TRUE(graph.getCompiledPasses().empty());
}

TEST_F(RenderGraphTest, CompilationDoesNotMutateDefinedPasses) {
  auto &pass = graph.addPass("Test");
  auto &pass1 = graph.addPass("Test2");
  auto &pass2 = graph.addPass("Test3");

  graph.compile(resourceRegistry);

  EXPECT_EQ(graph.getCompiledPasses().size(), 0);
  EXPECT_EQ(graph.getPasses().size(), 3);
  EXPECT_EQ(graph.getPasses().at(0).getName(), "Test");
  EXPECT_EQ(graph.getPasses().at(1).getName(), "Test2");
  EXPECT_EQ(graph.getPasses().at(2).getName(), "Test3");
}

TEST_F(RenderGraphTest, TopologicallySortRenderGraph) {
  //         +---+
  //         | H +--+  +---+
  //         +---+  +--> C |
  //  +---+            +-^-+---+
  //  | A |              |     |
  //  +-+-+    +---+     |   +-v-+
  //    | +----> B +-+---+   | E |<--+
  //    |      +^--+         +-+-+   |
  //    |       |  |   +---+   |     |
  //   +v--+    |  |   | F <---+     |
  // +-+ D +----+  |   +-+-+         |
  // | +-+-+     +-v-+   |           |
  // |   |   +---> G |<--+           |
  // |   +---+   +---+               |
  // +-------------------------------+

  std::unordered_map<liquid::String, liquid::rhi::TextureHandle> handles{
      {"a-b", resourceRegistry.setTexture({})},
      {"a-d", resourceRegistry.setTexture({})},
      {"d-b", resourceRegistry.setTexture({})},
      {"b-c", resourceRegistry.setTexture({})},
      {"b-g", resourceRegistry.setTexture({})},
      {"h-c", resourceRegistry.setTexture({})},
      {"c-e", resourceRegistry.setTexture({})},
      {"d-e", resourceRegistry.setTexture({})},
      {"d-g", resourceRegistry.setTexture({})},
      {"e-f", resourceRegistry.setTexture({})},
      {"f-g", resourceRegistry.setTexture({})},
      {"final-color", resourceRegistry.setTexture({})}};

  {
    auto &pass = graph.addPass("A");
    pass.write(handles.at("a-b"), glm::vec4());
    pass.write(handles.at("a-d"), glm::vec4());
  }

  {
    auto &pass = graph.addPass("B");
    pass.read(handles.at("a-b"));
    pass.read(handles.at("d-b"));
    pass.write(handles.at("b-c"), glm::vec4());
    pass.write(handles.at("b-g"), glm::vec4());
  }

  {
    auto &pass = graph.addPass("C");
    pass.read(handles.at("b-c"));
    pass.read(handles.at("h-c"));
    pass.write(handles.at("c-e"), glm::vec4());
  }

  {
    auto &pass = graph.addPass("D");
    pass.read(handles.at("a-d"));
    pass.write(handles.at("d-b"), glm::vec4());
    pass.write(handles.at("d-e"), glm::vec4());
    pass.write(handles.at("d-g"), glm::vec4());
  }

  {
    auto &pass = graph.addPass("E");
    pass.read(handles.at("d-e"));
    pass.read(handles.at("c-e"));
    pass.write(handles.at("e-f"), glm::vec4());
  }

  {
    auto &pass = graph.addPass("F");
    pass.read(handles.at("e-f"));
    pass.write(handles.at("f-g"), glm::vec4());
  }

  {
    auto &pass = graph.addPass("G");
    pass.read(handles.at("f-g"));
    pass.read(handles.at("d-g"));
    pass.read(handles.at("b-g"));
    pass.write(handles.at("final-color"), glm::vec4());
  }

  {
    auto &pass = graph.addPass("H");
    pass.write(handles.at("h-c"), glm::vec4());
  }

  graph.compile(resourceRegistry);

  // Join sorted pass names to a string
  // for easier assertion
  std::vector<liquid::String> names(graph.getPasses().size());
  std::transform(graph.getCompiledPasses().begin(),
                 graph.getCompiledPasses().end(), names.begin(),
                 [this](auto &pass) { return pass.getName(); });

  // Convert it to string for easier checking
  liquid::String output = "";
  for (auto &x : names) {
    output += x + " ";
  }

  // Trim last space
  output.erase(output.find_last_not_of(' ') + 1);

  EXPECT_EQ(output, "A D B H C E F G");
}

TEST_F(RenderGraphTest, SetsPassAttachmentOperations) {
  auto handle = resourceRegistry.setTexture({});

  graph.addPass("A").write(handle, glm::vec4());
  graph.addPass("B").write(handle, glm::vec4());
  graph.addPass("C").write(handle, glm::vec4());

  graph.compile(resourceRegistry);

  EXPECT_EQ(graph.getCompiledPasses().at(0).getAttachments().at(0).loadOp,
            liquid::rhi::AttachmentLoadOp::Clear);
  EXPECT_EQ(graph.getCompiledPasses().at(0).getAttachments().at(0).storeOp,
            liquid::rhi::AttachmentStoreOp::Store);

  EXPECT_EQ(graph.getCompiledPasses().at(1).getAttachments().at(0).loadOp,
            liquid::rhi::AttachmentLoadOp::Load);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getAttachments().at(0).storeOp,
            liquid::rhi::AttachmentStoreOp::Store);

  EXPECT_EQ(graph.getCompiledPasses().at(2).getAttachments().at(0).loadOp,
            liquid::rhi::AttachmentLoadOp::Load);
  EXPECT_EQ(graph.getCompiledPasses().at(2).getAttachments().at(0).storeOp,
            liquid::rhi::AttachmentStoreOp::Store);
}

TEST_F(RenderGraphTest, SetsOutputImageLayouts) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = resourceRegistry.setTexture(colorDescription);

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = resourceRegistry.setTexture(depthDescription);

  {
    auto &pass = graph.addPass("A");
    pass.write(colorTexture, glm::vec4{});
    pass.write(depthTexture, glm::vec4{});
  }

  {
    auto &pass = graph.addPass("B");
    pass.write(depthTexture, {});
    pass.write(colorTexture, {});
  }

  graph.compile(resourceRegistry);

  EXPECT_EQ(graph.getCompiledPasses().at(0).getOutputs().at(0).srcLayout,
            VK_IMAGE_LAYOUT_UNDEFINED);
  EXPECT_EQ(graph.getCompiledPasses().at(0).getOutputs().at(0).dstLayout,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  EXPECT_EQ(graph.getCompiledPasses().at(0).getOutputs().at(1).srcLayout,
            VK_IMAGE_LAYOUT_UNDEFINED);
  EXPECT_EQ(graph.getCompiledPasses().at(0).getOutputs().at(1).dstLayout,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

  EXPECT_EQ(graph.getCompiledPasses().at(1).getOutputs().at(0).srcLayout,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getOutputs().at(0).dstLayout,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getOutputs().at(1).srcLayout,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getOutputs().at(1).dstLayout,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

TEST_F(RenderGraphTest, SetsInputImageLayouts) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = resourceRegistry.setTexture(colorDescription);

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = resourceRegistry.setTexture(depthDescription);

  {
    auto &pass = graph.addPass("A");
    pass.write(colorTexture, glm::vec4{});
    pass.write(depthTexture, glm::vec4{});
  }

  {
    auto &pass = graph.addPass("B");
    pass.read(depthTexture);
    pass.read(colorTexture);
  }

  graph.compile(resourceRegistry);

  EXPECT_EQ(graph.getCompiledPasses().at(1).getInputs().at(0).srcLayout,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getInputs().at(0).dstLayout,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getInputs().at(1).srcLayout,
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getInputs().at(1).dstLayout,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

TEST_F(RenderGraphTest, SetsPassBarrierForColorOutput) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = resourceRegistry.setTexture(colorDescription);

  {
    auto &pass = graph.addPass("A");
    pass.write(colorTexture, glm::vec4{});
  }

  graph.compile(resourceRegistry);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(0).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_FALSE(preBarrier.enabled);
    EXPECT_TRUE(postBarrier.enabled);
    EXPECT_EQ(postBarrier.srcStage,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    EXPECT_EQ(postBarrier.dstStage,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForDepthOutput) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = resourceRegistry.setTexture(depthDescription);

  {
    auto &pass = graph.addPass("A");
    pass.write(depthTexture, glm::vec4{});
  }

  graph.compile(resourceRegistry);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(0).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_FALSE(preBarrier.enabled);
    EXPECT_TRUE(postBarrier.enabled);

    EXPECT_EQ(postBarrier.srcStage,
              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(postBarrier.dstStage,
              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT);
  }
}

TEST_F(RenderGraphTest, SetsBothBarriersFromOutputs) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = resourceRegistry.setTexture(colorDescription);
  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = resourceRegistry.setTexture(depthDescription);

  {
    auto &pass = graph.addPass("A");
    pass.write(colorTexture, glm::vec4{});
    pass.write(depthTexture, glm::vec4{});
  }

  graph.compile(resourceRegistry);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(0).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_FALSE(preBarrier.enabled);
    EXPECT_TRUE(postBarrier.enabled);

    EXPECT_EQ(postBarrier.srcStage,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(postBarrier.dstStage,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).srcAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).dstAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierFromColorInput) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = resourceRegistry.setTexture(colorDescription);

  {
    auto &pass = graph.addPass("A");
    pass.write(colorTexture, glm::vec4{});
  }

  {
    auto &pass = graph.addPass("B");
    pass.read(colorTexture);
  }

  graph.compile(resourceRegistry);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(1).getPostBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    EXPECT_EQ(preBarrier.dstStage, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    EXPECT_TRUE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.imageBarriers.size(), 1);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).texture, colorTexture);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcLayout,
              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstAccess,
              VK_ACCESS_SHADER_READ_BIT);

    EXPECT_TRUE(postBarrier.enabled);
    EXPECT_EQ(postBarrier.srcStage, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    EXPECT_EQ(postBarrier.dstStage,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    EXPECT_TRUE(postBarrier.memoryBarriers.empty());
    EXPECT_EQ(postBarrier.imageBarriers.size(), 1);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).texture, colorTexture);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstLayout,
              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcAccess,
              VK_ACCESS_SHADER_READ_BIT);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierFromDepthInput) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = resourceRegistry.setTexture(depthDescription);

  {
    auto &pass = graph.addPass("A");
    pass.write(depthTexture, glm::vec4{});
  }

  {
    auto &pass = graph.addPass("B");
    pass.read(depthTexture);
  }

  graph.compile(resourceRegistry);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(1).getPostBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage,
              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(preBarrier.dstStage, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    EXPECT_TRUE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.imageBarriers.size(), 1);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).texture, depthTexture);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcLayout,
              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstAccess,
              VK_ACCESS_SHADER_READ_BIT);

    EXPECT_TRUE(postBarrier.enabled);
    EXPECT_EQ(postBarrier.srcStage, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    EXPECT_EQ(postBarrier.dstStage,
              VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_TRUE(postBarrier.memoryBarriers.empty());
    EXPECT_EQ(postBarrier.imageBarriers.size(), 1);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).texture, depthTexture);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstLayout,
              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcAccess,
              VK_ACCESS_SHADER_READ_BIT);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierFromBothColorAndDepthInputs) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = resourceRegistry.setTexture(colorDescription);
  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = resourceRegistry.setTexture(depthDescription);

  {
    auto &pass = graph.addPass("A");
    pass.write(colorTexture, glm::vec4{});
    pass.write(depthTexture, glm::vec4{});
  }

  {
    auto &pass = graph.addPass("B");
    pass.read(colorTexture);
    pass.read(depthTexture);
  }

  graph.compile(resourceRegistry);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(1).getPostBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(preBarrier.dstStage, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    EXPECT_TRUE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.imageBarriers.size(), 2);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).texture, colorTexture);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcLayout,
              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstAccess,
              VK_ACCESS_SHADER_READ_BIT);

    EXPECT_EQ(preBarrier.imageBarriers.at(1).texture, depthTexture);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).srcLayout,
              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).dstLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).srcAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).dstAccess,
              VK_ACCESS_SHADER_READ_BIT);

    EXPECT_TRUE(postBarrier.enabled);
    EXPECT_EQ(postBarrier.srcStage, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    EXPECT_EQ(postBarrier.dstStage,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_TRUE(postBarrier.memoryBarriers.empty());
    EXPECT_EQ(postBarrier.imageBarriers.size(), 2);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).texture, colorTexture);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstLayout,
              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcAccess,
              VK_ACCESS_SHADER_READ_BIT);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    EXPECT_EQ(postBarrier.imageBarriers.at(1).texture, depthTexture);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).srcLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).dstLayout,
              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).srcAccess,
              VK_ACCESS_SHADER_READ_BIT);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).dstAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
  }
}

TEST_F(RenderGraphTest, MergesInputAndOutputBarriers) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture1 = resourceRegistry.setTexture(colorDescription);
  auto colorTexture2 = resourceRegistry.setTexture(colorDescription);

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture1 = resourceRegistry.setTexture(depthDescription);
  auto depthTexture2 = resourceRegistry.setTexture(depthDescription);

  {
    auto &pass = graph.addPass("A");
    pass.write(colorTexture1, glm::vec4{});
    pass.write(depthTexture1, glm::vec4{});
  }

  {
    auto &pass = graph.addPass("B");
    pass.read(colorTexture1);
    pass.read(depthTexture1);

    pass.write(depthTexture2, {});
    pass.write(colorTexture2, {});
  }

  graph.compile(resourceRegistry);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(1).getPostBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(preBarrier.dstStage, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    EXPECT_TRUE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.imageBarriers.size(), 2);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).texture, colorTexture1);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcLayout,
              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstAccess,
              VK_ACCESS_SHADER_READ_BIT);

    EXPECT_EQ(preBarrier.imageBarriers.at(1).texture, depthTexture1);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).srcLayout,
              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).dstLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).srcAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).dstAccess,
              VK_ACCESS_SHADER_READ_BIT);

    EXPECT_TRUE(postBarrier.enabled);
    EXPECT_EQ(postBarrier.srcStage,
              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(postBarrier.dstStage,
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT);
    EXPECT_EQ(postBarrier.imageBarriers.size(), 2);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).texture, colorTexture1);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstLayout,
              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcAccess,
              VK_ACCESS_SHADER_READ_BIT);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);

    EXPECT_EQ(postBarrier.imageBarriers.at(1).texture, depthTexture1);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).srcLayout,
              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).dstLayout,
              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).srcAccess,
              VK_ACCESS_SHADER_READ_BIT);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).dstAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

    EXPECT_EQ(postBarrier.memoryBarriers.size(), 2);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess,
              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).srcAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).dstAccess,
              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT);
  }
}

TEST_F(RenderGraphDeathTest, FailsIfPassReadsFromNonWrittenTexture) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = resourceRegistry.setTexture(colorDescription);

  {
    auto &pass = graph.addPass("A");
    pass.read(colorTexture);
  }

  EXPECT_DEATH(graph.compile(resourceRegistry), ".*");
}

TEST_F(RenderGraphTest, CompilationRemovesLonelyNodes) {
  liquid::rhi::TextureHandle handle = resourceRegistry.setTexture({});

  graph.addPass("A").write(handle, glm::vec4());
  graph.addPass("B");
  graph.addPass("C");
  graph.addPass("E").read(handle);

  graph.compile(resourceRegistry);

  EXPECT_EQ(graph.getCompiledPasses().size(), 2);
  EXPECT_EQ(graph.getPasses().size(), 4);
}

TEST_F(RenderGraphDeathTest, CompilationFailsIfMultipleNodesHaveTheSameName) {
  liquid::rhi::TextureHandle handle{2};

  graph.addPass("A").write(handle, glm::vec4());
  graph.addPass("B");
  graph.addPass("A");
  graph.addPass("E");

  EXPECT_DEATH(graph.compile(resourceRegistry), ".*");
}
