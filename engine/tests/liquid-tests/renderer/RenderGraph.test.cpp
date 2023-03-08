#include "liquid/core/Base.h"
#include "liquid/renderer/RenderGraph.h"

#include "liquid-tests/mocks/MockRenderDevice.h"
#include "liquid-tests/Testing.h"

using namespace liquid::rhi;

class RenderGraphTest : public ::testing::Test {
public:
  RenderGraphTest() : graph("TestGraph") {}

  MockRenderDevice device;
  liquid::RenderGraph graph;
};

class RenderGraphDeathTest : public RenderGraphTest {};

TEST_F(RenderGraphTest, AddsGraphicsPass) {
  auto &pass = graph.addGraphicsPass("Test");
  EXPECT_EQ(pass.getName(), "Test");
  EXPECT_EQ(pass.getType(), liquid::RenderGraphPassType::Graphics);
  EXPECT_EQ(graph.getPasses().at(0).getName(), "Test");
  EXPECT_EQ(graph.getPasses().at(0).getType(),
            liquid::RenderGraphPassType::Graphics);

  EXPECT_TRUE(graph.getCompiledPasses().empty());
}

TEST_F(RenderGraphTest, AddsComputePass) {
  auto &pass = graph.addComputePass("Test");
  EXPECT_EQ(pass.getName(), "Test");
  EXPECT_EQ(pass.getType(), liquid::RenderGraphPassType::Compute);
  EXPECT_EQ(graph.getPasses().at(0).getName(), "Test");
  EXPECT_EQ(graph.getPasses().at(0).getType(),
            liquid::RenderGraphPassType::Compute);

  EXPECT_TRUE(graph.getCompiledPasses().empty());
}

TEST_F(RenderGraphTest, CompilationDoesNotMutateDefinedPasses) {
  auto &pass = graph.addGraphicsPass("Test");
  auto &pass1 = graph.addGraphicsPass("Test2");
  auto &pass2 = graph.addComputePass("Test3");

  graph.compile(&device);

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

  std::unordered_map<liquid::String, liquid::rhi::TextureHandle> textures{
      {"a-d", device.createTexture({})},
      {"b-c", device.createTexture({})},
      {"b-g", device.createTexture({})},
      {"h-c", device.createTexture({})},
      {"c-e", device.createTexture({})},
      {"d-e", device.createTexture({})},
      {"d-g", device.createTexture({})},
      {"e-f", device.createTexture({})},
      {"f-g", device.createTexture({})},
      {"final-color", device.createTexture({})}};

  std::unordered_map<liquid::String, liquid::rhi::BufferHandle> buffers{
      {"a-b", device.createBuffer({}).getHandle()},
      {"d-b", device.createBuffer({}).getHandle()}};

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffers.at("a-b"), liquid::rhi::BufferUsage::Storage);
    pass.write(textures.at("a-d"), glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(buffers.at("a-b"), liquid::rhi::BufferUsage::Vertex);
    pass.read(buffers.at("d-b"), liquid::rhi::BufferUsage::Index);
    pass.write(textures.at("b-c"), glm::vec4());
    pass.write(textures.at("b-g"), glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("C");
    pass.read(textures.at("b-c"));
    pass.read(textures.at("h-c"));
    pass.write(textures.at("c-e"), glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("D");
    pass.read(textures.at("a-d"));
    pass.write(buffers.at("d-b"), liquid::rhi::BufferUsage::Uniform);
    pass.write(textures.at("d-e"), glm::vec4());
    pass.write(textures.at("d-g"), glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("E");
    pass.read(textures.at("d-e"));
    pass.read(textures.at("c-e"));
    pass.write(textures.at("e-f"), glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("F");
    pass.read(textures.at("e-f"));
    pass.write(textures.at("f-g"), glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("G");
    pass.read(textures.at("f-g"));
    pass.read(textures.at("d-g"));
    pass.read(textures.at("b-g"));
    pass.write(textures.at("final-color"), glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("H");
    pass.write(textures.at("h-c"), glm::vec4());
  }

  graph.compile(&device);

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
  auto handle = device.createTexture({});

  graph.addGraphicsPass("A").write(handle, glm::vec4());
  graph.addGraphicsPass("B").write(handle, glm::vec4());
  graph.addGraphicsPass("C").write(handle, glm::vec4());

  graph.compile(&device);

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
  auto colorTexture = device.createTexture(colorDescription);

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = device.createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, glm::vec4{});
    pass.write(depthTexture, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.write(depthTexture, {});
    pass.write(colorTexture, {});
  }

  graph.compile(&device);

  EXPECT_EQ(graph.getCompiledPasses().at(0).getTextureOutputs().at(0).srcLayout,
            ImageLayout::Undefined);
  EXPECT_EQ(graph.getCompiledPasses().at(0).getTextureOutputs().at(0).dstLayout,
            ImageLayout::ColorAttachmentOptimal);
  EXPECT_EQ(graph.getCompiledPasses().at(0).getTextureOutputs().at(1).srcLayout,
            ImageLayout::Undefined);
  EXPECT_EQ(graph.getCompiledPasses().at(0).getTextureOutputs().at(1).dstLayout,
            ImageLayout::DepthStencilAttachmentOptimal);

  EXPECT_EQ(graph.getCompiledPasses().at(1).getTextureOutputs().at(0).srcLayout,
            ImageLayout::DepthStencilAttachmentOptimal);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getTextureOutputs().at(0).dstLayout,
            ImageLayout::DepthStencilAttachmentOptimal);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getTextureOutputs().at(1).srcLayout,
            ImageLayout::ColorAttachmentOptimal);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getTextureOutputs().at(1).dstLayout,
            ImageLayout::ColorAttachmentOptimal);
}

TEST_F(RenderGraphTest, SetsInputImageLayouts) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = device.createTexture(colorDescription);

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = device.createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, glm::vec4{});
    pass.write(depthTexture, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(depthTexture);
    pass.read(colorTexture);
  }

  graph.compile(&device);

  EXPECT_EQ(graph.getCompiledPasses().at(1).getTextureInputs().at(0).srcLayout,
            ImageLayout::DepthStencilAttachmentOptimal);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getTextureInputs().at(0).dstLayout,
            ImageLayout::ShaderReadOnlyOptimal);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getTextureInputs().at(1).srcLayout,
            ImageLayout::ColorAttachmentOptimal);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getTextureInputs().at(1).dstLayout,
            ImageLayout::ShaderReadOnlyOptimal);
}

TEST_F(RenderGraphTest, SetsPassBarrierForColorWrite) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = device.createTexture(colorDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, glm::vec4{});
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(0).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_FALSE(preBarrier.enabled);
    EXPECT_TRUE(postBarrier.enabled);
    EXPECT_EQ(postBarrier.srcStage, PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess,
              Access::ColorAttachmentWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess,
              Access::ColorAttachmentWrite | Access::ColorAttachmentRead);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForDepthWrite) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = device.createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(depthTexture, glm::vec4{});
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(0).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_FALSE(preBarrier.enabled);
    EXPECT_TRUE(postBarrier.enabled);

    EXPECT_EQ(postBarrier.srcStage, PipelineStage::EarlyFragmentTests |
                                        PipelineStage::LateFragmentTests);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::EarlyFragmentTests |
                                        PipelineStage::LateFragmentTests);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess,
              Access::DepthStencilAttachmentWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess,
              Access::DepthStencilAttachmentWrite |
                  Access::DepthStencilAttachmentRead);
  }
}

TEST_F(RenderGraphTest, SetsPassBarriersForAllTextureWrites) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = device.createTexture(colorDescription);
  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = device.createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, glm::vec4{});
    pass.write(depthTexture, glm::vec4{});
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(0).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_FALSE(preBarrier.enabled);
    EXPECT_TRUE(postBarrier.enabled);

    EXPECT_EQ(postBarrier.srcStage, PipelineStage::ColorAttachmentOutput |
                                        PipelineStage::EarlyFragmentTests |
                                        PipelineStage::LateFragmentTests);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::ColorAttachmentOutput |
                                        PipelineStage::EarlyFragmentTests |
                                        PipelineStage::LateFragmentTests);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess,
              Access::ColorAttachmentWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess,
              Access::ColorAttachmentWrite | Access::ColorAttachmentRead);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).srcAccess,
              Access::DepthStencilAttachmentWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).dstAccess,
              Access::DepthStencilAttachmentWrite |
                  Access::DepthStencilAttachmentRead);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForColorRead) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = device.createTexture(colorDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(colorTexture);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(1).getPostBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::FragmentShader);
    EXPECT_TRUE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.imageBarriers.size(), 1);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).texture, colorTexture);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcLayout,
              ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcAccess,
              Access::ColorAttachmentWrite);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstAccess, Access::ShaderRead);

    EXPECT_TRUE(postBarrier.enabled);
    EXPECT_EQ(postBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::ColorAttachmentOutput);
    EXPECT_TRUE(postBarrier.memoryBarriers.empty());
    EXPECT_EQ(postBarrier.imageBarriers.size(), 1);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).texture, colorTexture);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstLayout,
              ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcAccess, Access::ShaderRead);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstAccess,
              Access::ColorAttachmentWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForDepthTextureRead) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = device.createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(depthTexture, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(depthTexture);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(1).getPostBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::EarlyFragmentTests |
                                       PipelineStage::LateFragmentTests);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::FragmentShader);
    EXPECT_TRUE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.imageBarriers.size(), 1);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).texture, depthTexture);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcLayout,
              ImageLayout::DepthStencilAttachmentOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcAccess,
              Access::DepthStencilAttachmentWrite);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstAccess, Access::ShaderRead);

    EXPECT_TRUE(postBarrier.enabled);
    EXPECT_EQ(postBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::EarlyFragmentTests |
                                        PipelineStage::LateFragmentTests);
    EXPECT_TRUE(postBarrier.memoryBarriers.empty());
    EXPECT_EQ(postBarrier.imageBarriers.size(), 1);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).texture, depthTexture);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstLayout,
              ImageLayout::DepthStencilAttachmentOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcAccess, Access::ShaderRead);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstAccess,
              Access::DepthStencilAttachmentWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForBothColorAndDepthTextureReads) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = device.createTexture(colorDescription);
  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = device.createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, glm::vec4{});
    pass.write(depthTexture, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(colorTexture);
    pass.read(depthTexture);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(1).getPostBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::ColorAttachmentOutput |
                                       PipelineStage::EarlyFragmentTests |
                                       PipelineStage::LateFragmentTests);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::FragmentShader);
    EXPECT_TRUE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.imageBarriers.size(), 2);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).texture, colorTexture);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcLayout,
              ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcAccess,
              Access::ColorAttachmentWrite);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstAccess, Access::ShaderRead);

    EXPECT_EQ(preBarrier.imageBarriers.at(1).texture, depthTexture);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).srcLayout,
              ImageLayout::DepthStencilAttachmentOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).dstLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).srcAccess,
              Access::DepthStencilAttachmentWrite);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).dstAccess, Access::ShaderRead);

    EXPECT_TRUE(postBarrier.enabled);
    EXPECT_EQ(postBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::ColorAttachmentOutput |
                                        PipelineStage::EarlyFragmentTests |
                                        PipelineStage::LateFragmentTests);
    EXPECT_TRUE(postBarrier.memoryBarriers.empty());
    EXPECT_EQ(postBarrier.imageBarriers.size(), 2);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).texture, colorTexture);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstLayout,
              ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcAccess, Access::ShaderRead);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstAccess,
              Access::ColorAttachmentWrite);

    EXPECT_EQ(postBarrier.imageBarriers.at(1).texture, depthTexture);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).srcLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).dstLayout,
              ImageLayout::DepthStencilAttachmentOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).srcAccess, Access::ShaderRead);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).dstAccess,
              Access::DepthStencilAttachmentWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForUniformBufferReadInGraphicsPass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addGraphicsPass("C");
    pass.read(buffer1, liquid::rhi::BufferUsage::Uniform);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::FragmentShader);
    EXPECT_EQ(preBarrier.memoryBarriers.size(), 1);
    EXPECT_FALSE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).dstAccess, Access::ShaderRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForUniformBufferReadInComputePass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addComputePass("C");
    pass.read(buffer1, liquid::rhi::BufferUsage::Uniform);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::ComputeShader);
    EXPECT_EQ(preBarrier.memoryBarriers.size(), 1);
    EXPECT_FALSE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).dstAccess, Access::ShaderRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForStorageBufferReadInGraphicsPass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::FragmentShader);
    EXPECT_EQ(preBarrier.memoryBarriers.size(), 1);
    EXPECT_FALSE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).dstAccess, Access::ShaderRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForStorageBufferReadInComputePass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addComputePass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addComputePass("B");
    pass.read(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::ComputeShader);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::ComputeShader);
    EXPECT_EQ(preBarrier.memoryBarriers.size(), 1);
    EXPECT_FALSE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).dstAccess, Access::ShaderRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForVertexBufferReadInGraphicsPass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(buffer1, liquid::rhi::BufferUsage::Vertex);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::VertexInput);
    EXPECT_EQ(preBarrier.memoryBarriers.size(), 1);
    EXPECT_FALSE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).dstAccess,
              Access::VertexAttributeRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForIndexBufferReadInGraphicsPass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(buffer1, liquid::rhi::BufferUsage::Index);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::VertexInput);
    EXPECT_EQ(preBarrier.memoryBarriers.size(), 1);
    EXPECT_FALSE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).dstAccess, Access::IndexRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForIndirectBufferReadInGraphicsPass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addComputePass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addGraphicsPass("C");
    pass.read(buffer1, liquid::rhi::BufferUsage::Indirect);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::ComputeShader);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::DrawIndirect);
    EXPECT_EQ(preBarrier.memoryBarriers.size(), 1);
    EXPECT_FALSE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).dstAccess,
              Access::IndirectCommandRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForAllBufferReadsInGraphicsPass) {
  auto buffer1 = device.createBuffer({}).getHandle();
  auto buffer2 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addComputePass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
    pass.write(buffer2, liquid::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addGraphicsPass("C");
    pass.read(buffer1, liquid::rhi::BufferUsage::Indirect |
                           liquid::rhi::BufferUsage::Vertex |
                           liquid::rhi::BufferUsage::Index);
    pass.read(buffer2, liquid::rhi::BufferUsage::Uniform |
                           liquid::rhi::BufferUsage::Storage);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::ComputeShader);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::DrawIndirect |
                                       PipelineStage::VertexInput |
                                       PipelineStage::FragmentShader);
    EXPECT_EQ(preBarrier.memoryBarriers.size(), 2);
    EXPECT_FALSE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).dstAccess,
              Access::IndirectCommandRead | Access::VertexAttributeRead |
                  Access::IndexRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
    EXPECT_EQ(preBarrier.memoryBarriers.at(1).dstAccess, Access::ShaderRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(1).srcAccess, Access::ShaderWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForAllBufferReadsInComputePass) {
  auto buffer1 = device.createBuffer({}).getHandle();
  auto buffer2 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
    pass.write(buffer2, liquid::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addComputePass("C");
    pass.read(buffer1, liquid::rhi::BufferUsage::Indirect);
    pass.read(buffer2, liquid::rhi::BufferUsage::Uniform |
                           liquid::rhi::BufferUsage::Storage);
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(preBarrier.dstStage,
              PipelineStage::DrawIndirect | PipelineStage::ComputeShader);
    EXPECT_EQ(preBarrier.memoryBarriers.size(), 2);
    EXPECT_FALSE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).dstAccess,
              Access::IndirectCommandRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
    EXPECT_EQ(preBarrier.memoryBarriers.at(1).dstAccess, Access::ShaderRead);
    EXPECT_EQ(preBarrier.memoryBarriers.at(1).srcAccess, Access::ShaderWrite);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForUniformBufferWriteInGraphicsPass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Uniform);
  }

  graph.compile(&device);

  {
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_TRUE(postBarrier.enabled);

    EXPECT_EQ(postBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.memoryBarriers.size(), 1);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess, Access::ShaderRead);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForUniformBufferWriteInComputePass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addComputePass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Uniform);
  }

  graph.compile(&device);

  {
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_TRUE(postBarrier.enabled);

    EXPECT_EQ(postBarrier.srcStage, PipelineStage::ComputeShader);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::ComputeShader);
    EXPECT_EQ(postBarrier.memoryBarriers.size(), 1);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess, Access::ShaderRead);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForStorageBufferWriteInGraphicsPass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  graph.compile(&device);

  {
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_TRUE(postBarrier.enabled);

    EXPECT_EQ(postBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.memoryBarriers.size(), 1);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess, Access::ShaderRead);
  }
}

TEST_F(RenderGraphTest, SetsPassBarrierForStorageBufferWriteInComputePass) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addComputePass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
  }

  graph.compile(&device);

  {
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_TRUE(postBarrier.enabled);

    EXPECT_EQ(postBarrier.srcStage, PipelineStage::ComputeShader);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::ComputeShader);
    EXPECT_EQ(postBarrier.memoryBarriers.size(), 1);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess, Access::ShaderRead);
  }
}

TEST_F(RenderGraphTest, SetsPassBarriersForAllBufferWritesInGraphicsPass) {
  auto buffer1 = device.createBuffer({}).getHandle();
  auto buffer2 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
    pass.write(buffer1, liquid::rhi::BufferUsage::Uniform);
  }

  graph.compile(&device);

  {
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_TRUE(postBarrier.enabled);

    EXPECT_EQ(postBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.memoryBarriers.size(), 2);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess, Access::ShaderRead);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).srcAccess, Access::ShaderWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).dstAccess, Access::ShaderRead);
  }
}

TEST_F(RenderGraphTest, SetsPassBarriersForAllBufferWritesInComputePass) {
  auto buffer1 = device.createBuffer({}).getHandle();
  auto buffer2 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, liquid::rhi::BufferUsage::Storage);
    pass.write(buffer1, liquid::rhi::BufferUsage::Uniform);
  }

  graph.compile(&device);

  {
    const auto &postBarrier = graph.getCompiledPasses().at(0).getPostBarrier();
    EXPECT_TRUE(postBarrier.enabled);

    EXPECT_EQ(postBarrier.srcStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::FragmentShader);
    EXPECT_EQ(postBarrier.memoryBarriers.size(), 2);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess, Access::ShaderWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess, Access::ShaderRead);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).srcAccess, Access::ShaderWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).dstAccess, Access::ShaderRead);
  }
}

TEST_F(RenderGraphTest, MergesInputAndOutputBarriers) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture1 = device.createTexture(colorDescription);
  auto colorTexture2 = device.createTexture(colorDescription);

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture1 = device.createTexture(depthDescription);
  auto depthTexture2 = device.createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture1, glm::vec4{});
    pass.write(depthTexture1, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(colorTexture1);
    pass.read(depthTexture1);

    pass.write(depthTexture2, {});
    pass.write(colorTexture2, {});
  }

  graph.compile(&device);

  {
    const auto &preBarrier = graph.getCompiledPasses().at(1).getPreBarrier();
    const auto &postBarrier = graph.getCompiledPasses().at(1).getPostBarrier();
    EXPECT_TRUE(preBarrier.enabled);

    EXPECT_EQ(preBarrier.srcStage, PipelineStage::ColorAttachmentOutput |
                                       PipelineStage::EarlyFragmentTests |
                                       PipelineStage::LateFragmentTests);
    EXPECT_EQ(preBarrier.dstStage, PipelineStage::FragmentShader);
    EXPECT_TRUE(preBarrier.memoryBarriers.empty());
    EXPECT_EQ(preBarrier.imageBarriers.size(), 2);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).texture, colorTexture1);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcLayout,
              ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).srcAccess,
              Access::ColorAttachmentWrite);
    EXPECT_EQ(preBarrier.imageBarriers.at(0).dstAccess, Access::ShaderRead);

    EXPECT_EQ(preBarrier.imageBarriers.at(1).texture, depthTexture1);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).srcLayout,
              ImageLayout::DepthStencilAttachmentOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).dstLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).srcAccess,
              Access::DepthStencilAttachmentWrite);
    EXPECT_EQ(preBarrier.imageBarriers.at(1).dstAccess, Access::ShaderRead);

    EXPECT_TRUE(postBarrier.enabled);
    EXPECT_EQ(postBarrier.srcStage, PipelineStage::FragmentShader |
                                        PipelineStage::ColorAttachmentOutput |
                                        PipelineStage::EarlyFragmentTests |
                                        PipelineStage::LateFragmentTests);
    EXPECT_EQ(postBarrier.dstStage, PipelineStage::ColorAttachmentOutput |
                                        PipelineStage::EarlyFragmentTests |
                                        PipelineStage::LateFragmentTests);
    EXPECT_EQ(postBarrier.imageBarriers.size(), 2);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).texture, colorTexture1);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstLayout,
              ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).srcAccess, Access::ShaderRead);
    EXPECT_EQ(postBarrier.imageBarriers.at(0).dstAccess,
              Access::ColorAttachmentWrite);

    EXPECT_EQ(postBarrier.imageBarriers.at(1).texture, depthTexture1);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).srcLayout,
              ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).dstLayout,
              ImageLayout::DepthStencilAttachmentOptimal);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).srcAccess, Access::ShaderRead);
    EXPECT_EQ(postBarrier.imageBarriers.at(1).dstAccess,
              Access::DepthStencilAttachmentWrite);

    EXPECT_EQ(postBarrier.memoryBarriers.size(), 2);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).srcAccess,
              Access::DepthStencilAttachmentWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(0).dstAccess,
              Access::DepthStencilAttachmentWrite |
                  Access::DepthStencilAttachmentRead);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).srcAccess,
              Access::ColorAttachmentWrite);
    EXPECT_EQ(postBarrier.memoryBarriers.at(1).dstAccess,
              Access::ColorAttachmentWrite | Access::ColorAttachmentRead);
  }
}

TEST_F(RenderGraphDeathTest, FailsIfPassReadsFromNonWrittenTexture) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = device.createTexture(colorDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.read(colorTexture);
  }

  EXPECT_DEATH(graph.compile(&device), ".*");
}

TEST_F(RenderGraphTest, CompilationRemovesLonelyNodes) {
  liquid::rhi::TextureHandle handle = device.createTexture({});

  graph.addGraphicsPass("A").write(handle, glm::vec4());
  graph.addGraphicsPass("B");
  graph.addGraphicsPass("C");
  graph.addGraphicsPass("E").read(handle);

  graph.compile(&device);

  EXPECT_EQ(graph.getCompiledPasses().size(), 2);
  EXPECT_EQ(graph.getPasses().size(), 4);
}

TEST_F(RenderGraphTest, RecompilationRecreatesCompiledPassesList) {
  liquid::rhi::TextureHandle handle = device.createTexture({});

  graph.addGraphicsPass("A").write(handle, glm::vec4());
  graph.addGraphicsPass("E").read(handle);

  graph.compile(&device);
  EXPECT_EQ(graph.getCompiledPasses().size(), 2);

  graph.setFramebufferExtent({});

  graph.compile(&device);
  EXPECT_EQ(graph.getCompiledPasses().size(), 2);
}

TEST_F(RenderGraphDeathTest, CompilationFailsIfMultipleNodesHaveTheSameName) {
  liquid::rhi::TextureHandle handle{2};

  graph.addGraphicsPass("A").write(handle, glm::vec4());
  graph.addGraphicsPass("B");
  graph.addGraphicsPass("A");
  graph.addGraphicsPass("E");

  EXPECT_DEATH(graph.compile(&device), ".*");
}
