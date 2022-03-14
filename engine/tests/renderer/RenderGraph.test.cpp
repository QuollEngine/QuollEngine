#include "liquid/core/Base.h"
#include "liquid/renderer/render-graph/RenderGraph.h"

#include <gtest/gtest.h>

struct EmptyScope {};

constexpr auto noopExecutor = [](liquid::rhi::RenderCommandList &commandList,
                                 EmptyScope &scope,
                                 liquid::RenderGraphRegistry &registry) {};

class RenderGraphTest : public ::testing::Test {
public:
  liquid::RenderGraph graph;
};

class RenderGraphDeathTest : public RenderGraphTest {};

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

  graph.create("a-b", {});
  graph.create("a-d", {});
  graph.create("d-b", {});
  graph.create("b-c", {});
  graph.create("b-g", {});
  graph.create("h-c", {});
  graph.create("c-e", {});
  graph.create("d-e", {});
  graph.create("d-g", {});
  graph.create("e-f", {});
  graph.create("f-g", {});

  // A
  graph.addInlinePass<EmptyScope>(
      "A",
      [](auto &builder, auto &scope) {
        builder.write("a-b");
        builder.write("a-d");
      },
      noopExecutor);

  // B
  graph.addInlinePass<EmptyScope>(
      "B",
      [](auto &builder, auto &scope) {
        builder.read("a-b");
        builder.read("d-b");
        builder.write("b-c");
        builder.write("b-g");
      },
      noopExecutor);

  // C
  graph.addInlinePass<EmptyScope>(
      "C",
      [](auto &builder, auto &scope) {
        builder.read("b-c");
        builder.read("h-c");
        builder.write("c-e");
      },
      noopExecutor);

  // D
  graph.addInlinePass<EmptyScope>(
      "D",
      [](auto &builder, auto &scope) {
        builder.read("a-d");
        builder.write("d-b");
        builder.write("d-e");
        builder.write("d-g");
      },
      noopExecutor);

  // E
  graph.addInlinePass<EmptyScope>(
      "E",
      [](auto &builder, auto &scope) {
        builder.read("d-e");
        builder.read("c-e");
        builder.write("e-f");
      },
      noopExecutor);

  // F
  graph.addInlinePass<EmptyScope>(
      "F",
      [](auto &builder, auto &scope) {
        builder.read("e-f");
        builder.write("f-g");
      },
      noopExecutor);

  // G
  graph.addInlinePass<EmptyScope>(
      "G",
      [](auto &builder, auto &scope) {
        builder.read("f-g");
        builder.read("d-g");
        builder.read("b-g");
        builder.write("SWAPCHAIN");
      },
      noopExecutor);

  // H
  graph.addInlinePass<EmptyScope>(
      "H", [](auto &builder, auto &scope) { builder.write("h-c"); },
      noopExecutor);

  const auto &sortedPasses = graph.compile();

  // Join sorted pass names to a string
  // for easier assertion
  std::vector<liquid::String> names(sortedPasses.size());
  std::transform(
      sortedPasses.begin(), sortedPasses.end(), names.begin(),
      [](liquid::RenderGraphPassBase *pass) { return pass->getName(); });

  // Convert it to string for easier checking
  liquid::String output = "";
  for (auto &x : names) {
    output += x + " ";
  }

  // Trim last space
  output.erase(output.find_last_not_of(' ') + 1);

  EXPECT_EQ(output, "A D B H C E F G");
}

TEST_F(RenderGraphTest, SetsPassLoadOperations) {
  auto resourceId = graph.create("a-b", {});
  graph.addInlinePass<EmptyScope>(
      "A", [](auto &builder, auto &scope) { builder.write("a-b"); },
      noopExecutor);
  graph.addInlinePass<EmptyScope>(
      "B", [](auto &builder, auto &scope) { builder.write("a-b"); },
      noopExecutor);
  graph.addInlinePass<EmptyScope>(
      "C", [](auto &builder, auto &scope) { builder.write("a-b"); },
      noopExecutor);

  const auto &sortedPasses = graph.compile();
  EXPECT_EQ(sortedPasses.at(0)->getOutputs().at(resourceId).loadOp,
            liquid::AttachmentLoadOp::Clear);
  EXPECT_EQ(sortedPasses.at(1)->getOutputs().at(resourceId).loadOp,
            liquid::AttachmentLoadOp::Load);
  EXPECT_EQ(sortedPasses.at(2)->getOutputs().at(resourceId).loadOp,
            liquid::AttachmentLoadOp::Load);
}

TEST_F(RenderGraphTest, SetPassClearValues) {
  constexpr glm::vec4 SWAPCHAIN_CLEAR{1.0, 0.0, 1.0, 1.0};
  constexpr liquid::DepthStencilClear DEPTH_CLEAR{1.0f, 5};

  graph.setSwapchainColor(SWAPCHAIN_CLEAR);
  liquid::AttachmentData data;
  data.clearValue = DEPTH_CLEAR;

  auto depthId = graph.create("depthBuffer", data);
  auto swapchainId = graph.getResourceId("SWAPCHAIN");
  graph.addInlinePass<EmptyScope>(
      "a-b",
      [](auto &builder, auto &scope) {
        builder.write("SWAPCHAIN");
        builder.write("depthBuffer");
      },
      noopExecutor);
  graph.addInlinePass<EmptyScope>(
      "b-c",
      [](auto &builder, auto &scope) {
        builder.write("SWAPCHAIN");
        builder.write("depthBuffer");
      },
      noopExecutor);

  const auto &sortedPasses = graph.compile();
  for (const auto &pass : sortedPasses) {
    EXPECT_TRUE(
        std::get<glm::vec4>(pass->getOutputs().at(swapchainId).clearValue) ==
        SWAPCHAIN_CLEAR);
    EXPECT_TRUE(std::get<liquid::DepthStencilClear>(
                    pass->getOutputs().at(depthId).clearValue)
                    .clearDepth == DEPTH_CLEAR.clearDepth);
    EXPECT_TRUE(std::get<liquid::DepthStencilClear>(
                    pass->getOutputs().at(depthId).clearValue)
                    .clearStencil == DEPTH_CLEAR.clearStencil);
  }
}

TEST_F(RenderGraphTest, CompilationRemovesLonelyNodes) {
  graph.create("a-b", {});

  graph.addInlinePass<EmptyScope>(
      "A", [](auto &builder, auto &scope) { builder.write("a-b"); },
      noopExecutor);

  graph.addInlinePass<EmptyScope>(
      "B", [](auto &builder, auto &scope) {}, noopExecutor);

  graph.addInlinePass<EmptyScope>(
      "C", [](auto &builder, auto &scope) {}, noopExecutor);

  graph.addInlinePass<EmptyScope>(
      "E", [](auto &builder, auto &scope) { builder.read("a-b"); },
      noopExecutor);

  EXPECT_EQ(graph.compile().size(), 2);
}

TEST_F(RenderGraphDeathTest, BuildOnlyCalledOnce) {
  std::set_terminate([]() { FAIL(); });

  graph.create("a-b", {});

  graph.addInlinePass<EmptyScope>(
      "A", [](auto &builder, auto &scope) { builder.write("a-b"); },
      noopExecutor);

  EXPECT_EXIT(
      {
        graph.compile();
        graph.compile();
        exit(0);
      },
      ::testing::ExitedWithCode(0), ".*");
}

TEST_F(RenderGraphDeathTest, CompilationFailsIfMultipleNodesHaveTheSameName) {
  graph.create("a-b", {});

  graph.addInlinePass<EmptyScope>(
      "A", [](auto &builder, auto &scope) { builder.write("a-b"); },
      noopExecutor);

  graph.addInlinePass<EmptyScope>(
      "B", [](auto &builder, auto &scope) {}, noopExecutor);

  graph.addInlinePass<EmptyScope>(
      "A", [](auto &builder, auto &scope) {}, noopExecutor);

  graph.addInlinePass<EmptyScope>(
      "E", [](auto &builder, auto &scope) { builder.read("a-b"); },
      noopExecutor);

  EXPECT_DEATH(graph.compile(), ".*");
}
