#include "liquid/core/Base.h"
#include "liquid/rhi-mock/MockRenderDevice.h"

#include "liquid/renderer/RenderGraph.h"

#include "liquid-tests/Testing.h"

using namespace liquid::rhi;

class RenderGraphTest : public ::testing::Test {
public:
  RenderGraphTest() : graph("TestGraph"), storage(&device) {}

  liquid::RenderGraphResource<TextureHandle>
  createTexture(liquid::rhi::TextureDescription desc) {
    return graph.create(desc, [](auto, auto &) {});
  }

  MockRenderDevice device;
  liquid::RenderStorage storage;
  liquid::RenderGraph graph;

  using OnBuildMockFunction =
      ::testing::MockFunction<void(TextureHandle, liquid::RenderStorage &)>;
};

using RenderGraphDeathTest = RenderGraphTest;

TEST_F(RenderGraphTest, CreatesAllTexturesOnBuild) {
  OnBuildMockFunction onCreateR1;
  OnBuildMockFunction onCreateR2;

  EXPECT_CALL(onCreateR1, Call(::testing::_, ::testing::_)).Times(1);
  EXPECT_CALL(onCreateR2, Call(::testing::_, ::testing::_)).Times(1);

  TextureDescription fixedDescription{};
  fixedDescription.width = 512;
  fixedDescription.height = 512;

  graph.setFramebufferExtent({1920, 1080});

  auto r1 = graph.create(fixedDescription, onCreateR1.AsStdFunction());
  auto r2 = graph.create(
      [](auto width, auto height) {
        TextureDescription desc{};
        desc.width = width;
        desc.height = height;

        return desc;
      },
      onCreateR2.AsStdFunction());

  EXPECT_FALSE(isHandleValid(r1.getHandle()));
  EXPECT_FALSE(isHandleValid(r2.getHandle()));

  graph.build(storage);

  EXPECT_TRUE(isHandleValid(r1.getHandle()));
  EXPECT_TRUE(isHandleValid(r2.getHandle()));

  auto desc1 = device.getTextureDescription(r1.getHandle());
  EXPECT_EQ(desc1.width, 512);
  EXPECT_EQ(desc1.height, 512);

  auto desc2 = device.getTextureDescription(r2.getHandle());
  EXPECT_EQ(desc2.width, 1920);
  EXPECT_EQ(desc2.height, 1080);
}

TEST_F(RenderGraphTest, RecreatesDynamicTexturesOnResize) {
  OnBuildMockFunction onCreateR1;
  OnBuildMockFunction onCreateR2;

  EXPECT_CALL(onCreateR1, Call(::testing::_, ::testing::_)).Times(1);
  EXPECT_CALL(onCreateR2, Call(::testing::_, ::testing::_)).Times(2);

  TextureDescription fixedDescription{};
  fixedDescription.width = 512;
  fixedDescription.height = 512;

  graph.setFramebufferExtent({1920, 1080});

  auto r1 = graph.create(fixedDescription, onCreateR1.AsStdFunction());
  auto r2 = graph.create(
      [](auto width, auto height) {
        TextureDescription desc{};
        desc.width = width;
        desc.height = height;

        return desc;
      },
      onCreateR2.AsStdFunction());

  graph.build(storage);

  auto desc1 = device.getTextureDescription(r1.getHandle());
  EXPECT_EQ(desc1.width, 512);
  EXPECT_EQ(desc1.height, 512);
  EXPECT_EQ(device.getTextureUpdates(r1.getHandle()), 1);

  auto desc2 = device.getTextureDescription(r2.getHandle());
  EXPECT_EQ(desc2.width, 1920);
  EXPECT_EQ(desc2.height, 1080);
  EXPECT_EQ(device.getTextureUpdates(r2.getHandle()), 1);

  graph.setFramebufferExtent({2560, 1440});
  graph.build(storage);

  EXPECT_EQ(device.getTextureUpdates(r1.getHandle()), 1);

  auto desc3 = device.getTextureDescription(r2.getHandle());
  EXPECT_EQ(desc3.width, 2560);
  EXPECT_EQ(desc3.height, 1440);
  EXPECT_EQ(device.getTextureUpdates(r2.getHandle()), 2);
}

TEST_F(RenderGraphTest, ImportsExternalResourcesToRenderGraph) {
  auto handle = storage.createTexture({});

  auto resource = graph.import(handle);
  EXPECT_EQ(device.getTextureUpdates(handle), 1);
  EXPECT_TRUE(isHandleValid(handle));
  EXPECT_EQ(resource.getHandle(), handle);

  graph.build(storage);
  EXPECT_EQ(device.getTextureUpdates(handle), 1);
  EXPECT_EQ(resource.getHandle(), handle);
}

TEST_F(RenderGraphTest, IgnoresExternalResourcesOnResize) {
  auto handle = storage.createTexture({});

  auto resource = graph.import(handle);
  EXPECT_EQ(device.getTextureUpdates(handle), 1);
  EXPECT_EQ(resource.getHandle(), handle);
  EXPECT_TRUE(isHandleValid(handle));

  graph.build(storage);
  EXPECT_EQ(device.getTextureUpdates(handle), 1);
  EXPECT_EQ(resource.getHandle(), handle);

  graph.setFramebufferExtent({2560, 1440});
  EXPECT_EQ(device.getTextureUpdates(handle), 1);
  EXPECT_EQ(resource.getHandle(), handle);
}

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

  graph.build(storage);

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

  using Texture = TextureHandle;

  auto finalColor = storage.createTexture({});

  std::unordered_map<liquid::String, liquid::RenderGraphResource<TextureHandle>>
      textures{{"a-d", createTexture({})},
               {"b-c", createTexture({})},
               {"b-g", createTexture({})},
               {"h-c", createTexture({})},
               {"c-e", createTexture({})},
               {"d-e", createTexture({})},
               {"d-g", createTexture({})},
               {"e-f", createTexture({})},
               {"f-g", createTexture({})},
               {"final-color", graph.import(finalColor)}};

  std::unordered_map<liquid::String, liquid::rhi::BufferHandle> buffers{
      {"a-b", device.createBuffer({}).getHandle()},
      {"d-b", device.createBuffer({}).getHandle()}};

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffers.at("a-b"), liquid::rhi::BufferUsage::Storage);
    pass.write(textures.at("a-d"), liquid::AttachmentType::Color, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(buffers.at("a-b"), liquid::rhi::BufferUsage::Vertex);
    pass.read(buffers.at("d-b"), liquid::rhi::BufferUsage::Index);
    pass.write(textures.at("b-c"), liquid::AttachmentType::Color, glm::vec4());
    pass.write(textures.at("b-g"), liquid::AttachmentType::Depth, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("C");
    pass.read(textures.at("b-c"));
    pass.read(textures.at("h-c"));
    pass.write(textures.at("c-e"), liquid::AttachmentType::Color, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("D");
    pass.read(textures.at("a-d"));
    pass.write(buffers.at("d-b"), liquid::rhi::BufferUsage::Uniform);
    pass.write(textures.at("d-e"), liquid::AttachmentType::Color, glm::vec4());
    pass.write(textures.at("d-g"), liquid::AttachmentType::Color, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("E");
    pass.read(textures.at("d-e"));
    pass.read(textures.at("c-e"));
    pass.write(textures.at("e-f"), liquid::AttachmentType::Color, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("F");
    pass.read(textures.at("e-f"));
    pass.write(textures.at("f-g"), liquid::AttachmentType::Resolve,
               glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("G");
    pass.read(textures.at("f-g"));
    pass.read(textures.at("d-g"));
    pass.read(textures.at("b-g"));
    pass.write(textures.at("final-color"), liquid::AttachmentType::Color,
               glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("H");
    pass.write(textures.at("h-c"), liquid::AttachmentType::Depth, glm::vec4());
  }

  graph.build(storage);

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
  auto handle = createTexture({});

  graph.addGraphicsPass("A").write(handle, liquid::AttachmentType::Color,
                                   glm::vec4());
  graph.addGraphicsPass("B").write(handle, liquid::AttachmentType::Color,
                                   glm::vec4());
  graph.addGraphicsPass("C").write(handle, liquid::AttachmentType::Color,
                                   glm::vec4());

  graph.build(storage);

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
  auto colorTexture = createTexture(colorDescription);

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, liquid::AttachmentType::Color, glm::vec4{});
    pass.write(depthTexture, liquid::AttachmentType::Depth, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.write(depthTexture, liquid::AttachmentType::Depth, {});
    pass.write(colorTexture, liquid::AttachmentType::Color, {});
  }

  graph.build(storage);

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
  auto colorTexture = createTexture(colorDescription);

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, liquid::AttachmentType::Color, glm::vec4{});
    pass.write(depthTexture, liquid::AttachmentType::Depth, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(depthTexture);
    pass.read(colorTexture);
  }

  graph.build(storage);

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
  auto colorTexture = createTexture(colorDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, liquid::AttachmentType::Color, glm::vec4{});
  }

  graph.build(storage);

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
  auto depthTexture = createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(depthTexture, liquid::AttachmentType::Depth, glm::vec4{});
  }

  graph.build(storage);

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
  auto colorTexture = createTexture(colorDescription);
  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, liquid::AttachmentType::Color, glm::vec4{});
    pass.write(depthTexture, liquid::AttachmentType::Depth, glm::vec4{});
  }

  graph.build(storage);

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
  auto colorTexture = createTexture(colorDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, liquid::AttachmentType::Color, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(colorTexture);
  }

  graph.build(storage);

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
  auto depthTexture = createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(depthTexture, liquid::AttachmentType::Depth, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(depthTexture);
  }

  graph.build(storage);

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
  auto colorTexture = createTexture(colorDescription);
  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture = createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, liquid::AttachmentType::Color, glm::vec4{});
    pass.write(depthTexture, liquid::AttachmentType::Depth, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(colorTexture);
    pass.read(depthTexture);
  }

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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

  graph.build(storage);

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
  auto colorTexture1 = createTexture(colorDescription);
  auto colorTexture2 = createTexture(colorDescription);

  TextureDescription depthDescription{};
  depthDescription.usage = TextureUsage::Depth;
  auto depthTexture1 = createTexture(depthDescription);
  auto depthTexture2 = createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture1, liquid::AttachmentType::Color, glm::vec4{});
    pass.write(depthTexture1, liquid::AttachmentType::Depth, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(colorTexture1);
    pass.read(depthTexture1);

    pass.write(depthTexture2, liquid::AttachmentType::Depth, {});
    pass.write(colorTexture2, liquid::AttachmentType::Color, {});
  }

  graph.build(storage);

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

TEST_F(RenderGraphTest, BuildsRenderPassWithOnlyColorAttachments) {
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  colorDescription.width = 1024;
  colorDescription.height = 768;
  colorDescription.layers = 10;
  auto texture = createTexture(colorDescription);

  auto &pass = graph.addGraphicsPass("A");
  pass.write(texture, liquid::AttachmentType::Color, glm::vec4{2.5f});

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  EXPECT_NE(compiled.getRenderPass(), RenderPassHandle::Invalid);
  EXPECT_NE(compiled.getFramebuffer(), FramebufferHandle::Invalid);

  auto renderPassDesc =
      device.getRenderPassDescription(compiled.getRenderPass());
  EXPECT_EQ(renderPassDesc.bindPoint, PipelineBindPoint::Graphics);
  EXPECT_EQ(renderPassDesc.colorAttachments.size(), 1);
  EXPECT_FALSE(renderPassDesc.depthAttachment.has_value());
  EXPECT_FALSE(renderPassDesc.resolveAttachment.has_value());
  EXPECT_EQ(renderPassDesc.colorAttachments.at(0).texture, texture);
  EXPECT_EQ(
      std::get<glm::vec4>(renderPassDesc.colorAttachments.at(0).clearValue),
      glm::vec4{2.5f});

  auto framebufferDesc =
      device.getFramebufferDescription(compiled.getFramebuffer());
  EXPECT_EQ(framebufferDesc.renderPass, compiled.getRenderPass());
  EXPECT_EQ(framebufferDesc.layers, 10);
  EXPECT_EQ(framebufferDesc.width, 1024);
  EXPECT_EQ(framebufferDesc.height, 768);
  EXPECT_EQ(framebufferDesc.attachments.size(), 1);
  EXPECT_EQ(framebufferDesc.attachments.at(0), texture);
}

TEST_F(RenderGraphTest, BuildsRenderPassWithOnlyDepthAttachment) {
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Depth;
  colorDescription.width = 1024;
  colorDescription.height = 768;
  colorDescription.layers = 10;
  auto texture = createTexture(colorDescription);

  auto &pass = graph.addGraphicsPass("A");
  pass.write(texture, liquid::AttachmentType::Depth,
             DepthStencilClear{2.5f, 35});

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  EXPECT_NE(compiled.getRenderPass(), RenderPassHandle::Invalid);
  EXPECT_NE(compiled.getFramebuffer(), FramebufferHandle::Invalid);

  auto renderPassDesc =
      device.getRenderPassDescription(compiled.getRenderPass());
  EXPECT_EQ(renderPassDesc.bindPoint, PipelineBindPoint::Graphics);
  EXPECT_EQ(renderPassDesc.colorAttachments.size(), 0);
  EXPECT_FALSE(renderPassDesc.resolveAttachment.has_value());
  EXPECT_TRUE(renderPassDesc.depthAttachment.has_value());
  EXPECT_EQ(renderPassDesc.depthAttachment.value().texture, texture);
  auto clear = std::get<DepthStencilClear>(
      renderPassDesc.depthAttachment.value().clearValue);
  EXPECT_EQ(clear.clearDepth, 2.5f);
  EXPECT_EQ(clear.clearStencil, 35);

  auto framebufferDesc =
      device.getFramebufferDescription(compiled.getFramebuffer());
  EXPECT_EQ(framebufferDesc.renderPass, compiled.getRenderPass());
  EXPECT_EQ(framebufferDesc.layers, 10);
  EXPECT_EQ(framebufferDesc.width, 1024);
  EXPECT_EQ(framebufferDesc.height, 768);
  EXPECT_EQ(framebufferDesc.attachments.size(), 1);
  EXPECT_EQ(framebufferDesc.attachments.at(0), texture);
}

TEST_F(RenderGraphTest, BuildsRenderPassWithOnlyResolveAttachment) {
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  colorDescription.width = 1024;
  colorDescription.height = 768;
  colorDescription.layers = 10;
  auto texture = createTexture(colorDescription);

  auto &pass = graph.addGraphicsPass("A");
  pass.write(texture, liquid::AttachmentType::Resolve, glm::vec4{2.5f});

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  EXPECT_NE(compiled.getRenderPass(), RenderPassHandle::Invalid);
  EXPECT_NE(compiled.getFramebuffer(), FramebufferHandle::Invalid);

  auto renderPassDesc =
      device.getRenderPassDescription(compiled.getRenderPass());
  EXPECT_EQ(renderPassDesc.bindPoint, PipelineBindPoint::Graphics);
  EXPECT_EQ(renderPassDesc.colorAttachments.size(), 0);
  EXPECT_TRUE(renderPassDesc.resolveAttachment.has_value());
  EXPECT_FALSE(renderPassDesc.depthAttachment.has_value());
  EXPECT_EQ(renderPassDesc.resolveAttachment.value().texture, texture);
  EXPECT_EQ(
      std::get<glm::vec4>(renderPassDesc.resolveAttachment.value().clearValue),
      glm::vec4{2.5f});

  auto framebufferDesc =
      device.getFramebufferDescription(compiled.getFramebuffer());
  EXPECT_EQ(framebufferDesc.renderPass, compiled.getRenderPass());
  EXPECT_EQ(framebufferDesc.layers, 10);
  EXPECT_EQ(framebufferDesc.width, 1024);
  EXPECT_EQ(framebufferDesc.height, 768);
  EXPECT_EQ(framebufferDesc.attachments.size(), 1);
  EXPECT_EQ(framebufferDesc.attachments.at(0), texture);
}

TEST_F(RenderGraphTest, BuildsRenderPassWithAllAttachments) {
  TextureDescription description{};
  description.usage = TextureUsage::Depth | TextureUsage::Color;
  description.width = 1024;
  description.height = 768;
  description.layers = 10;
  auto color1 = createTexture(description);
  auto color2 = createTexture(description);
  auto depth = createTexture(description);
  auto resolve = createTexture(description);

  auto &pass = graph.addGraphicsPass("A");
  pass.write(color1, liquid::AttachmentType::Color, glm::vec4{2.5f});
  pass.write(color2, liquid::AttachmentType::Color, glm::vec4{4.5f});
  pass.write(depth, liquid::AttachmentType::Depth, DepthStencilClear{2.5f, 35});
  pass.write(resolve, liquid::AttachmentType::Resolve, glm::vec4{5.5f});

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  auto renderPassDesc =
      device.getRenderPassDescription(compiled.getRenderPass());
  EXPECT_EQ(renderPassDesc.bindPoint, PipelineBindPoint::Graphics);
  EXPECT_EQ(renderPassDesc.colorAttachments.size(), 2);
  EXPECT_TRUE(renderPassDesc.resolveAttachment.has_value());
  EXPECT_TRUE(renderPassDesc.depthAttachment.has_value());

  EXPECT_EQ(renderPassDesc.colorAttachments.at(0).texture, color1);
  EXPECT_EQ(
      std::get<glm::vec4>(renderPassDesc.colorAttachments.at(0).clearValue),
      glm::vec4{2.5f});
  EXPECT_EQ(renderPassDesc.colorAttachments.at(1).texture, color2);
  EXPECT_EQ(
      std::get<glm::vec4>(renderPassDesc.colorAttachments.at(1).clearValue),
      glm::vec4{4.5f});

  auto clear = std::get<DepthStencilClear>(
      renderPassDesc.depthAttachment.value().clearValue);
  EXPECT_EQ(clear.clearDepth, 2.5f);
  EXPECT_EQ(clear.clearStencil, 35);

  EXPECT_EQ(renderPassDesc.resolveAttachment.value().texture, resolve);
  EXPECT_EQ(
      std::get<glm::vec4>(renderPassDesc.resolveAttachment.value().clearValue),
      glm::vec4{5.5f});

  auto framebufferDesc =
      device.getFramebufferDescription(compiled.getFramebuffer());
  EXPECT_EQ(framebufferDesc.renderPass, compiled.getRenderPass());
  EXPECT_EQ(framebufferDesc.layers, 10);
  EXPECT_EQ(framebufferDesc.width, 1024);
  EXPECT_EQ(framebufferDesc.height, 768);
  EXPECT_EQ(framebufferDesc.attachments.size(), 4);
  EXPECT_EQ(framebufferDesc.attachments.at(0), color1);
  EXPECT_EQ(framebufferDesc.attachments.at(1), color2);
  EXPECT_EQ(framebufferDesc.attachments.at(2), depth);
  EXPECT_EQ(framebufferDesc.attachments.at(3), resolve);
}

TEST_F(RenderGraphTest, BuildsGraphicsPipelinesForGraphicsPasses) {
  GraphicsPipelineDescription description{};
  description.vertexShader = ShaderHandle{25};

  auto pipeline = storage.addPipeline(description);

  auto texture = createTexture({});
  auto &pass = graph.addGraphicsPass("A");
  pass.write(texture, liquid::AttachmentType::Color, glm::vec4{0.0f});
  pass.addPipeline(pipeline);

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  EXPECT_NE(compiled.getRenderPass(), RenderPassHandle::Invalid);
  EXPECT_NE(compiled.getFramebuffer(), FramebufferHandle::Invalid);

  auto mock = device.getPipeline(pipeline);
  EXPECT_EQ(mock.getType(), MockPipeline::Type::Graphics);
  auto mockDescription = mock.getGraphicsDescription();

  EXPECT_EQ(mockDescription.vertexShader, ShaderHandle{25});
  EXPECT_EQ(mockDescription.renderPass, compiled.getRenderPass());
}

TEST_F(RenderGraphTest, BuildsComputePipelinesForComputePasses) {
  ComputePipelineDescription description{};
  description.computeShader = ShaderHandle{25};

  auto pipeline = storage.addPipeline(description);

  auto texture = createTexture({});
  auto &pass = graph.addComputePass("A");
  pass.write(texture, liquid::AttachmentType::Color, glm::vec4{0.0f});
  pass.addPipeline(pipeline);

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  auto mock = device.getPipeline(pipeline);
  EXPECT_EQ(mock.getType(), MockPipeline::Type::Compute);
  auto mockDescription = mock.getComputeDecription();

  EXPECT_EQ(mockDescription.computeShader, ShaderHandle{25});
}

TEST_F(RenderGraphDeathTest, FailsIfPassReadsFromNonWrittenTexture) {
  using TextureDescription = liquid::rhi::TextureDescription;
  using TextureUsage = liquid::rhi::TextureUsage;
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  auto colorTexture = createTexture(colorDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.read(colorTexture);
  }

  EXPECT_DEATH(graph.build(storage), ".*");
}

TEST_F(RenderGraphTest, CompilationRemovesLonelyNodes) {
  auto handle = createTexture({});

  graph.addGraphicsPass("A").write(handle, liquid::AttachmentType::Color,
                                   glm::vec4());
  graph.addGraphicsPass("B");
  graph.addGraphicsPass("C");
  graph.addGraphicsPass("E").read(handle);

  graph.build(storage);

  EXPECT_EQ(graph.getCompiledPasses().size(), 2);
  EXPECT_EQ(graph.getPasses().size(), 4);
}

TEST_F(RenderGraphTest, RecompilationRecreatesCompiledPassesList) {
  auto handle = createTexture({});

  graph.addGraphicsPass("A").write(handle, liquid::AttachmentType::Color,
                                   glm::vec4());
  graph.addGraphicsPass("E").read(handle);

  graph.build(storage);
  EXPECT_EQ(graph.getCompiledPasses().size(), 2);

  graph.setFramebufferExtent({});

  graph.build(storage);
  EXPECT_EQ(graph.getCompiledPasses().size(), 2);
}

TEST_F(RenderGraphDeathTest, CompilationFailsIfMultipleNodesHaveTheSameName) {
  auto handle = createTexture({});

  graph.addGraphicsPass("A").write(handle, liquid::AttachmentType::Color,
                                   glm::vec4());
  graph.addGraphicsPass("B");
  graph.addGraphicsPass("A");
  graph.addGraphicsPass("E");

  EXPECT_DEATH(graph.build(storage), ".*");
}
