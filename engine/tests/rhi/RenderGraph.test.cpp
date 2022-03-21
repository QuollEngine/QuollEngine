#include "liquid/core/Base.h"
#include "liquid/rhi/RenderGraph.h"

#include <gtest/gtest.h>

class RenderGraphTest : public ::testing::Test {
public:
  liquid::rhi::RenderGraph graph;
};

class RenderGraphDeathTest : public RenderGraphTest {};

TEST_F(RenderGraphTest, AddsPass) {
  auto &pass = graph.addPass("Test");
  EXPECT_EQ(pass.getName(), "Test");
  EXPECT_EQ(graph.getPasses().at(0).getName(), "Test");
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

  using TextureHandle = liquid::rhi::TextureHandle;

  std::unordered_map<liquid::String, liquid::rhi::TextureHandle> handles{
      {"a-b", TextureHandle(2)},  {"a-d", TextureHandle(3)},
      {"d-b", TextureHandle(4)},  {"b-c", TextureHandle(5)},
      {"b-g", TextureHandle(6)},  {"h-c", TextureHandle(7)},
      {"c-e", TextureHandle(8)},  {"d-e", TextureHandle(9)},
      {"d-g", TextureHandle(10)}, {"e-f", TextureHandle(11)},
      {"f-g", TextureHandle(12)}};

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
    pass.write(graph.getSwapchain(), glm::vec4());
  }

  {
    auto &pass = graph.addPass("H");
    pass.write(handles.at("h-c"), glm::vec4());
  }

  const auto &sortedPasses = graph.compile();

  // Join sorted pass names to a string
  // for easier assertion
  std::vector<liquid::String> names(sortedPasses.size());
  std::transform(
      sortedPasses.begin(), sortedPasses.end(), names.begin(),
      [this](size_t index) { return graph.getPasses().at(index).getName(); });

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
  liquid::rhi::TextureHandle handle{2};

  graph.addPass("A").write(handle, glm::vec4());
  graph.addPass("B").write(handle, glm::vec4());
  graph.addPass("C").write(handle, glm::vec4());

  const auto &sortedPasses = graph.compile();

  EXPECT_EQ(
      graph.getPasses().at(sortedPasses.at(0)).getAttachments().at(0).loadOp,
      liquid::rhi::AttachmentLoadOp::Clear);
  EXPECT_EQ(
      graph.getPasses().at(sortedPasses.at(0)).getAttachments().at(0).storeOp,
      liquid::rhi::AttachmentStoreOp::Store);

  EXPECT_EQ(
      graph.getPasses().at(sortedPasses.at(1)).getAttachments().at(0).loadOp,
      liquid::rhi::AttachmentLoadOp::Load);
  EXPECT_EQ(
      graph.getPasses().at(sortedPasses.at(1)).getAttachments().at(0).storeOp,
      liquid::rhi::AttachmentStoreOp::Store);

  EXPECT_EQ(
      graph.getPasses().at(sortedPasses.at(2)).getAttachments().at(0).loadOp,
      liquid::rhi::AttachmentLoadOp::Load);
  EXPECT_EQ(
      graph.getPasses().at(sortedPasses.at(2)).getAttachments().at(0).storeOp,
      liquid::rhi::AttachmentStoreOp::Store);
}

TEST_F(RenderGraphTest, CompilationRemovesLonelyNodes) {
  liquid::rhi::TextureHandle handle{2};

  graph.addPass("A").write(handle, glm::vec4());
  graph.addPass("B");
  graph.addPass("C");
  graph.addPass("E").read(handle);

  EXPECT_EQ(graph.compile().size(), 2);
}

TEST_F(RenderGraphDeathTest, CompilationFailsIfMultipleNodesHaveTheSameName) {
  liquid::rhi::TextureHandle handle{2};

  graph.addPass("A").write(handle, glm::vec4());
  graph.addPass("B");
  graph.addPass("A");
  graph.addPass("E");

  EXPECT_DEATH(graph.compile(), ".*");
}
