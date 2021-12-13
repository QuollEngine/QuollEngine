#include "core/Base.h"
#include "renderer/render-graph/RenderGraph.h"

#include <gtest/gtest.h>

struct EmptyScope {};

constexpr auto noopExecutor = [](liquid::RenderCommandList &commandList,
                                 EmptyScope &scope,
                                 liquid::RenderGraphRegistry &registry) {};
constexpr auto noopBuilder = [](liquid::RenderGraphBuilder &builder,
                                EmptyScope &scope) {};

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

  liquid::RenderGraph graph;

  // A
  graph.addPass<EmptyScope>(
      "A",
      [](auto &builder, auto &scope) {
        builder.write("a-b", {});
        builder.write("a-d", {});
      },
      noopExecutor);

  // B
  graph.addPass<EmptyScope>(
      "B",
      [](auto &builder, auto &scope) {
        builder.read("a-b");
        builder.read("d-b");
        builder.write("b-c", {});
        builder.write("b-g", {});
      },
      noopExecutor);

  // C
  graph.addPass<EmptyScope>(
      "C",
      [](auto &builder, auto &scope) {
        builder.read("b-c");
        builder.read("h-c");
        builder.write("c-e", {});
      },
      noopExecutor);

  // D
  graph.addPass<EmptyScope>(
      "D",
      [](auto &builder, auto &scope) {
        builder.read("a-d");
        builder.write("d-b", {});
        builder.write("d-e", {});
        builder.write("d-g", {});
      },
      noopExecutor);

  // E
  graph.addPass<EmptyScope>(
      "E",
      [](auto &builder, auto &scope) {
        builder.read("d-e");
        builder.read("c-e");
        builder.write("e-f", {});
      },
      noopExecutor);

  // F
  graph.addPass<EmptyScope>(
      "F",
      [](auto &builder, auto &scope) {
        builder.read("e-f");
        builder.writeSwapchain("f-g", {});
      },
      noopExecutor);

  // G
  graph.addPass<EmptyScope>(
      "G",
      [](auto &builder, auto &scope) {
        builder.read("f-g");
        builder.read("d-g");
        builder.read("b-g");
        builder.writeSwapchain("swapchain", {});
      },
      noopExecutor);

  // H
  graph.addPass<EmptyScope>(
      "H", [](auto &builder, auto &scope) { builder.write("h-c", {}); },
      noopExecutor);

  const auto &sortedPasses = graph.compile();

  // Join sorted pass names to a string
  // for easier assertion
  std::vector<liquid::String> names(sortedPasses.size());
  std::transform(
      sortedPasses.begin(), sortedPasses.end(), names.begin(),
      [](liquid::RenderGraphPassInterface *pass) { return pass->getName(); });

  // Convert it to string for easier checking
  liquid::String output = "";
  for (auto &x : names) {
    output += x + " ";
  }

  // Trim last space
  output.erase(output.find_last_not_of(' ') + 1);

  EXPECT_EQ(output, "H A D B C E F G");
}

TEST_F(RenderGraphTest, CompilationRemovesLonelyNodes) {
  liquid::RenderGraph graph;

  graph.addPass<EmptyScope>(
      "A", [](auto &builder, auto &scope) { builder.write("a-b", {}); },
      noopExecutor);

  graph.addPass<EmptyScope>(
      "B", [](auto &builder, auto &scope) {}, noopExecutor);

  graph.addPass<EmptyScope>(
      "C", [](auto &builder, auto &scope) {}, noopExecutor);

  graph.addPass<EmptyScope>(
      "E", [](auto &builder, auto &scope) { builder.read("a-b"); },
      noopExecutor);

  EXPECT_EQ(graph.compile().size(), 2);
}

TEST_F(RenderGraphDeathTest, BuildOnlyCalledOnce) {
  std::set_terminate([]() { FAIL(); });
  liquid::RenderGraph graph;

  graph.addPass<EmptyScope>(
      "A", [](auto &builder, auto &scope) { builder.write("a-b", {}); },
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
  liquid::RenderGraph graph;

  graph.addPass<EmptyScope>(
      "A", [](auto &builder, auto &scope) { builder.write("a-b", {}); },
      noopExecutor);

  graph.addPass<EmptyScope>(
      "B", [](auto &builder, auto &scope) {}, noopExecutor);

  graph.addPass<EmptyScope>(
      "A", [](auto &builder, auto &scope) {}, noopExecutor);

  graph.addPass<EmptyScope>(
      "E", [](auto &builder, auto &scope) { builder.read("a-b"); },
      noopExecutor);

  EXPECT_DEATH(graph.compile(), ".*");
}

TEST_F(RenderGraphDeathTest,
       CompilationFailsIfNodeInputDoesNotPointToResource) {
  liquid::RenderGraph graph;

  graph.addPass<EmptyScope>(
      "E", [](auto &builder, auto &scope) { builder.read("d-e"); },
      noopExecutor);

  EXPECT_DEATH(graph.compile(), ".*");
}
