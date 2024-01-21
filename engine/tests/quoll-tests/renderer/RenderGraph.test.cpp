#include "quoll/core/Base.h"
#include "quoll/profiler/MetricsCollector.h"
#include "quoll/renderer/RenderGraph.h"
#include "quoll/renderer/RenderStorage.h"
#include "quoll/rhi-mock/MockRenderDevice.h"
#include "quoll-tests/Testing.h"

using namespace quoll::rhi;

class RenderGraphTest : public ::testing::Test {
public:
  RenderGraphTest() : graph("TestGraph"), storage(&device, metricsCollector) {}

  quoll::RenderGraphResource<TextureHandle>
  createTexture(quoll::rhi::TextureDescription desc) {
    return graph.create(desc);
  }

  MockRenderDevice device;
  quoll::MetricsCollector metricsCollector;
  quoll::RenderStorage storage;
  quoll::RenderGraph graph;

  using OnBuildMockFunction =
      ::testing::MockFunction<void(TextureHandle, quoll::RenderStorage &)>;
};

using RenderGraphDeathTest = RenderGraphTest;

TEST_F(RenderGraphTest, CreatesAllTexturesOnBuild) {
  OnBuildMockFunction onCreateR1;
  OnBuildMockFunction onCreateR2;

  EXPECT_CALL(onCreateR1, Call(::testing::_, ::testing::_)).Times(1);
  EXPECT_CALL(onCreateR2, Call(::testing::_, ::testing::_)).Times(1);

  TextureDescription fixedDescription{};
  fixedDescription.width = 1920;
  fixedDescription.height = 1080;

  auto t1 = graph.create(fixedDescription).onReady(onCreateR1.AsStdFunction());
  auto t2 =
      graph.createView(t1, 1, 2, 3, 4).onReady(onCreateR2.AsStdFunction());

  EXPECT_FALSE(isHandleValid(t1.getHandle()));
  EXPECT_FALSE(isHandleValid(t2.getHandle()));

  graph.build(storage);

  EXPECT_TRUE(isHandleValid(t1.getHandle()));
  EXPECT_TRUE(isHandleValid(t2.getHandle()));

  auto desc1 = device.getTextureDescription(t1.getHandle());
  EXPECT_EQ(desc1.width, 1920);
  EXPECT_EQ(desc1.height, 1080);

  auto desc2 = device.getTextureViewDescription(t2.getHandle());
  EXPECT_EQ(desc2.texture, t1.getHandle());
  EXPECT_EQ(desc2.baseMipLevel, 1);
  EXPECT_EQ(desc2.mipLevelCount, 2);
  EXPECT_EQ(desc2.baseLayer, 3);
  EXPECT_EQ(desc2.layerCount, 4);
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

TEST_F(RenderGraphTest, AddsGraphicsPass) {
  auto &pass = graph.addGraphicsPass("Test");
  EXPECT_EQ(pass.getName(), "Test");
  EXPECT_EQ(pass.getType(), quoll::RenderGraphPassType::Graphics);
  EXPECT_EQ(graph.getPasses().at(0).getName(), "Test");
  EXPECT_EQ(graph.getPasses().at(0).getType(),
            quoll::RenderGraphPassType::Graphics);

  EXPECT_TRUE(graph.getCompiledPasses().empty());
}

TEST_F(RenderGraphTest, AddsComputePass) {
  auto &pass = graph.addComputePass("Test");
  EXPECT_EQ(pass.getName(), "Test");
  EXPECT_EQ(pass.getType(), quoll::RenderGraphPassType::Compute);
  EXPECT_EQ(graph.getPasses().at(0).getName(), "Test");
  EXPECT_EQ(graph.getPasses().at(0).getType(),
            quoll::RenderGraphPassType::Compute);

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

  std::unordered_map<quoll::String, quoll::RenderGraphResource<TextureHandle>>
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

  std::unordered_map<quoll::String, quoll::rhi::BufferHandle> buffers{
      {"a-b", device.createBuffer({}).getHandle()},
      {"d-b", device.createBuffer({}).getHandle()}};

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffers.at("a-b"), quoll::rhi::BufferUsage::Storage);
    pass.write(textures.at("a-d"), quoll::AttachmentType::Color, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(buffers.at("a-b"), quoll::rhi::BufferUsage::Vertex);
    pass.read(buffers.at("d-b"), quoll::rhi::BufferUsage::Index);
    pass.write(textures.at("b-c"), quoll::AttachmentType::Color, glm::vec4());
    pass.write(textures.at("b-g"), quoll::AttachmentType::Depth, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("C");
    pass.read(textures.at("b-c"));
    pass.read(textures.at("h-c"));
    pass.write(textures.at("c-e"), quoll::AttachmentType::Color, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("D");
    pass.read(textures.at("a-d"));
    pass.write(buffers.at("d-b"), quoll::rhi::BufferUsage::Uniform);
    pass.write(textures.at("d-e"), quoll::AttachmentType::Color, glm::vec4());
    pass.write(textures.at("d-g"), quoll::AttachmentType::Color, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("E");
    pass.read(textures.at("d-e"));
    pass.read(textures.at("c-e"));
    pass.write(textures.at("e-f"), quoll::AttachmentType::Color, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("F");
    pass.read(textures.at("e-f"));
    pass.write(textures.at("f-g"), quoll::AttachmentType::Resolve, glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("G");
    pass.read(textures.at("f-g"));
    pass.read(textures.at("d-g"));
    pass.read(textures.at("b-g"));
    pass.write(textures.at("final-color"), quoll::AttachmentType::Color,
               glm::vec4());
  }

  {
    auto &pass = graph.addGraphicsPass("H");
    pass.write(textures.at("h-c"), quoll::AttachmentType::Depth, glm::vec4());
  }

  graph.build(storage);

  // Join sorted pass names to a string
  // for easier assertion
  std::vector<quoll::String> names(graph.getPasses().size());
  std::transform(graph.getCompiledPasses().begin(),
                 graph.getCompiledPasses().end(), names.begin(),
                 [](auto &pass) { return pass.getName(); });

  // Convert it to string for easier checking
  quoll::String output = "";
  for (auto &x : names) {
    output += x + " ";
  }

  // Trim last space
  output.erase(output.find_last_not_of(' ') + 1);

  EXPECT_EQ(output, "A D B H C E F G");
}

TEST_F(RenderGraphTest, SetsPassAttachmentOperations) {
  auto handle = createTexture({});

  graph.addGraphicsPass("A").write(handle, quoll::AttachmentType::Color,
                                   glm::vec4());
  graph.addGraphicsPass("B").write(handle, quoll::AttachmentType::Color,
                                   glm::vec4());
  graph.addGraphicsPass("C").write(handle, quoll::AttachmentType::Color,
                                   glm::vec4());

  graph.build(storage);

  EXPECT_EQ(graph.getCompiledPasses().at(0).getAttachments().at(0).loadOp,
            quoll::rhi::AttachmentLoadOp::Clear);
  EXPECT_EQ(graph.getCompiledPasses().at(0).getAttachments().at(0).storeOp,
            quoll::rhi::AttachmentStoreOp::Store);

  EXPECT_EQ(graph.getCompiledPasses().at(1).getAttachments().at(0).loadOp,
            quoll::rhi::AttachmentLoadOp::Load);
  EXPECT_EQ(graph.getCompiledPasses().at(1).getAttachments().at(0).storeOp,
            quoll::rhi::AttachmentStoreOp::Store);

  EXPECT_EQ(graph.getCompiledPasses().at(2).getAttachments().at(0).loadOp,
            quoll::rhi::AttachmentLoadOp::Load);
  EXPECT_EQ(graph.getCompiledPasses().at(2).getAttachments().at(0).storeOp,
            quoll::rhi::AttachmentStoreOp::Store);
}

TEST_F(RenderGraphTest,
       SetsImageBarrierFromUndefinedBeforeFirstPassThatWritesToTheTexture) {
  quoll::rhi::TextureDescription colorDescription{};
  colorDescription.usage = quoll::rhi::TextureUsage::Color;
  auto colorTexture = createTexture(colorDescription);

  quoll::rhi::TextureDescription depthDescription{};
  depthDescription.usage = quoll::rhi::TextureUsage::Depth;
  auto depthTexture = createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, quoll::AttachmentType::Color, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(colorTexture);
    pass.write(depthTexture, quoll::AttachmentType::Depth, {});
  }

  graph.build(storage);

  auto dependencies0 = graph.getCompiledPasses().at(0).getSyncDependencies();
  EXPECT_EQ(dependencies0.imageBarriers.size(), 1);

  {
    const auto &imageBarrier = dependencies0.imageBarriers.at(0);
    EXPECT_EQ(imageBarrier.texture, colorTexture.getHandle());
    EXPECT_EQ(imageBarrier.srcStage, quoll::rhi::PipelineStage::None);
    EXPECT_EQ(imageBarrier.dstStage,
              quoll::rhi::PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::None);
    EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ColorAttachmentWrite);
    EXPECT_EQ(imageBarrier.srcLayout, quoll::rhi::ImageLayout::Undefined);
    EXPECT_EQ(imageBarrier.dstLayout,
              quoll::rhi::ImageLayout::ColorAttachmentOptimal);
  }

  const auto &dependencies1 =
      graph.getCompiledPasses().at(1).getSyncDependencies();
  EXPECT_EQ(dependencies1.imageBarriers.size(), 2);

  {
    const auto &imageBarrier = dependencies1.imageBarriers.at(0);

    EXPECT_EQ(imageBarrier.texture, depthTexture.getHandle());
    EXPECT_EQ(imageBarrier.srcStage, quoll::rhi::PipelineStage::None);
    EXPECT_EQ(imageBarrier.dstStage,
              quoll::rhi::PipelineStage::EarlyFragmentTests |
                  quoll::rhi::PipelineStage::LateFragmentTests);
    EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::None);
    EXPECT_EQ(imageBarrier.dstAccess,
              quoll::rhi::Access::DepthStencilAttachmentWrite);
    EXPECT_EQ(imageBarrier.srcLayout, quoll::rhi::ImageLayout::Undefined);
    EXPECT_EQ(imageBarrier.dstLayout,
              quoll::rhi::ImageLayout::DepthStencilAttachmentOptimal);
  }

  {
    const auto &imageBarrier = dependencies1.imageBarriers.at(1);

    EXPECT_EQ(imageBarrier.texture, colorTexture.getHandle());
    EXPECT_EQ(imageBarrier.srcStage,
              quoll::rhi::PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(imageBarrier.dstStage, quoll::rhi::PipelineStage::FragmentShader);
    EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::ColorAttachmentWrite);
    EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ShaderRead);
    EXPECT_EQ(imageBarrier.srcLayout,
              quoll::rhi::ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(imageBarrier.dstLayout,
              quoll::rhi::ImageLayout::ShaderReadOnlyOptimal);
  }
}

TEST_F(RenderGraphTest, SetsImageBarrierForAllMipLevelsIfTextureIsNotAView) {
  quoll::rhi::TextureDescription colorDescription{};
  colorDescription.usage = quoll::rhi::TextureUsage::Color;
  colorDescription.mipLevelCount = 20;
  auto colorTexture = createTexture(colorDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, quoll::AttachmentType::Color, glm::vec4{});
  }

  graph.build(storage);

  auto dependencies = graph.getCompiledPasses().at(0).getSyncDependencies();
  EXPECT_EQ(dependencies.imageBarriers.size(), 1);

  const auto &imageBarrier = dependencies.imageBarriers.at(0);
  EXPECT_EQ(imageBarrier.texture, colorTexture.getHandle());
  EXPECT_EQ(imageBarrier.baseLevel, 0);
  EXPECT_EQ(imageBarrier.levelCount, 20);
}

TEST_F(RenderGraphTest,
       SetsImageBarrierForSpecifiedMipLevelsWhenTextureIsView) {
  quoll::rhi::TextureDescription colorDescription{};
  colorDescription.usage = quoll::rhi::TextureUsage::Color;
  colorDescription.mipLevelCount = 20;
  auto colorTexture = createTexture(colorDescription);

  auto view = graph.createView(colorTexture, 5, 10);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(view, quoll::AttachmentType::Color, glm::vec4{});
  }

  graph.build(storage);

  auto dependencies = graph.getCompiledPasses().at(0).getSyncDependencies();
  EXPECT_EQ(dependencies.imageBarriers.size(), 1);

  const auto &imageBarrier = dependencies.imageBarriers.at(0);
  EXPECT_EQ(imageBarrier.texture, view.getHandle());
  EXPECT_EQ(imageBarrier.baseLevel, 5);
  EXPECT_EQ(imageBarrier.levelCount, 10);
}

TEST_F(RenderGraphTest, SetsImageBarrierBetweenPassWrites) {
  quoll::rhi::TextureDescription description{};
  description.usage = quoll::rhi::TextureUsage::Color;

  auto colorTexture = createTexture(description);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, quoll::AttachmentType::Color, glm::vec4{});
  }

  {
    auto &pass = graph.addComputePass("B");
    pass.write(colorTexture, quoll::AttachmentType::Color, {});
  }

  graph.build(storage);

  const auto &dependencies =
      graph.getCompiledPasses().at(1).getSyncDependencies();
  EXPECT_EQ(dependencies.imageBarriers.size(), 1);

  const auto &imageBarrier = dependencies.imageBarriers.at(0);
  EXPECT_EQ(imageBarrier.texture, colorTexture.getHandle());
  EXPECT_EQ(imageBarrier.srcStage,
            quoll::rhi::PipelineStage::ColorAttachmentOutput);
  EXPECT_EQ(imageBarrier.dstStage, quoll::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::ColorAttachmentWrite);
  EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ShaderWrite);
  EXPECT_EQ(imageBarrier.srcLayout,
            quoll::rhi::ImageLayout::ColorAttachmentOptimal);
  EXPECT_EQ(imageBarrier.dstLayout, quoll::rhi::ImageLayout::General);
}

TEST_F(RenderGraphTest,
       SetsMultipleImageBarriersWhenPassesWriteToMultipleTextures) {
  quoll::rhi::TextureDescription colorDescription{};
  colorDescription.usage = quoll::rhi::TextureUsage::Color;
  auto colorTexture = createTexture(colorDescription);

  quoll::rhi::TextureDescription depthDescription{};
  depthDescription.usage = quoll::rhi::TextureUsage::Depth;
  auto depthTexture = createTexture(depthDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, quoll::AttachmentType::Color, glm::vec4{});
    pass.write(depthTexture, quoll::AttachmentType::Depth, {});
  }

  {
    auto &pass = graph.addComputePass("B");
    pass.write(colorTexture, quoll::AttachmentType::Color, {});
    pass.write(depthTexture, quoll::AttachmentType::Depth, {});
  }

  graph.build(storage);

  const auto &dependencies =
      graph.getCompiledPasses().at(1).getSyncDependencies();
  EXPECT_EQ(dependencies.imageBarriers.size(), 2);

  {
    const auto &imageBarrier = dependencies.imageBarriers.at(0);
    EXPECT_EQ(imageBarrier.texture, colorTexture.getHandle());
    EXPECT_EQ(imageBarrier.srcStage,
              quoll::rhi::PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(imageBarrier.dstStage, quoll::rhi::PipelineStage::ComputeShader);
    EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::ColorAttachmentWrite);
    EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ShaderWrite);
    EXPECT_EQ(imageBarrier.srcLayout,
              quoll::rhi::ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(imageBarrier.dstLayout, quoll::rhi::ImageLayout::General);
  }

  {
    const auto &imageBarrier = dependencies.imageBarriers.at(1);

    EXPECT_EQ(imageBarrier.texture, depthTexture.getHandle());
    EXPECT_EQ(imageBarrier.srcStage,
              quoll::rhi::PipelineStage::EarlyFragmentTests |
                  quoll::rhi::PipelineStage::LateFragmentTests);
    EXPECT_EQ(imageBarrier.dstStage, quoll::rhi::PipelineStage::ComputeShader);
    EXPECT_EQ(imageBarrier.srcAccess,
              quoll::rhi::Access::DepthStencilAttachmentWrite);
    EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ShaderWrite);
    EXPECT_EQ(imageBarrier.srcLayout,
              quoll::rhi::ImageLayout::DepthStencilAttachmentOptimal);
    EXPECT_EQ(imageBarrier.dstLayout, quoll::rhi::ImageLayout::General);
  }
}

TEST_F(RenderGraphTest, SetsImageBarrierBetweenPassWriteAndPassRead) {
  quoll::rhi::TextureDescription colorDescription{};
  colorDescription.usage = quoll::rhi::TextureUsage::Color;
  auto colorTexture = createTexture(colorDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, quoll::AttachmentType::Color, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(colorTexture);
  }

  graph.build(storage);

  const auto &dependencies =
      graph.getCompiledPasses().at(1).getSyncDependencies();
  EXPECT_EQ(dependencies.imageBarriers.size(), 1);

  const auto &imageBarrier = dependencies.imageBarriers.at(0);
  EXPECT_EQ(imageBarrier.texture, colorTexture.getHandle());
  EXPECT_EQ(imageBarrier.srcStage,
            quoll::rhi::PipelineStage::ColorAttachmentOutput);
  EXPECT_EQ(imageBarrier.dstStage, quoll::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::ColorAttachmentWrite);
  EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ShaderRead);
  EXPECT_EQ(imageBarrier.srcLayout,
            quoll::rhi::ImageLayout::ColorAttachmentOptimal);
  EXPECT_EQ(imageBarrier.dstLayout,
            quoll::rhi::ImageLayout::ShaderReadOnlyOptimal);
}

TEST_F(RenderGraphTest, SetsImageBarriersBetweenPassReads) {
  quoll::rhi::TextureDescription colorDescription{};
  colorDescription.usage = quoll::rhi::TextureUsage::Color;
  auto colorTexture = createTexture(colorDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, quoll::AttachmentType::Color, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(colorTexture);
  }

  {
    auto &pass = graph.addComputePass("C");
    pass.read(colorTexture);
  }

  graph.build(storage);

  const auto &dependencies1 =
      graph.getCompiledPasses().at(1).getSyncDependencies();
  EXPECT_EQ(dependencies1.imageBarriers.size(), 1);

  {
    const auto &imageBarrier = dependencies1.imageBarriers.at(0);
    EXPECT_EQ(imageBarrier.texture, colorTexture.getHandle());
    EXPECT_EQ(imageBarrier.srcStage,
              quoll::rhi::PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(imageBarrier.dstStage, quoll::rhi::PipelineStage::FragmentShader);
    EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::ColorAttachmentWrite);
    EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ShaderRead);
    EXPECT_EQ(imageBarrier.srcLayout,
              quoll::rhi::ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(imageBarrier.dstLayout,
              quoll::rhi::ImageLayout::ShaderReadOnlyOptimal);
  }

  auto dependencies2 = graph.getCompiledPasses().at(2).getSyncDependencies();
  EXPECT_EQ(dependencies2.imageBarriers.size(), 1);

  {
    const auto &imageBarrier = dependencies2.imageBarriers.at(0);

    EXPECT_EQ(imageBarrier.texture, colorTexture.getHandle());
    EXPECT_EQ(imageBarrier.srcStage, quoll::rhi::PipelineStage::FragmentShader);
    EXPECT_EQ(imageBarrier.dstStage, quoll::rhi::PipelineStage::ComputeShader);
    EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::ShaderRead);
    EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ShaderRead);
    EXPECT_EQ(imageBarrier.srcLayout,
              quoll::rhi::ImageLayout::ShaderReadOnlyOptimal);
    EXPECT_EQ(imageBarrier.dstLayout,
              quoll::rhi::ImageLayout::ShaderReadOnlyOptimal);
  }
}

TEST_F(RenderGraphTest, SetsImageBarrierBetweenPassReadAndPassWrite) {
  quoll::rhi::TextureDescription colorDescription{};
  colorDescription.usage = quoll::rhi::TextureUsage::Color;
  auto colorTexture = createTexture(colorDescription);

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(colorTexture, quoll::AttachmentType::Color, glm::vec4{});
  }

  {
    auto &pass = graph.addGraphicsPass("B");
    pass.read(colorTexture);
    pass.write(colorTexture, quoll::AttachmentType::Color, {});
  }

  graph.build(storage);

  const auto &dependencies =
      graph.getCompiledPasses().at(1).getSyncDependencies();
  EXPECT_EQ(dependencies.imageBarriers.size(), 2);

  {
    const auto &imageBarrier = dependencies.imageBarriers.at(0);

    EXPECT_EQ(imageBarrier.texture, colorTexture.getHandle());
    EXPECT_EQ(imageBarrier.srcStage,
              quoll::rhi::PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(imageBarrier.dstStage,
              quoll::rhi::PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::ColorAttachmentWrite);
    EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ColorAttachmentWrite);
    EXPECT_EQ(imageBarrier.srcLayout,
              quoll::rhi::ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(imageBarrier.dstLayout,
              quoll::rhi::ImageLayout::ColorAttachmentOptimal);
  }

  {
    const auto &imageBarrier = dependencies.imageBarriers.at(1);

    EXPECT_EQ(imageBarrier.texture, colorTexture.getHandle());
    EXPECT_EQ(imageBarrier.srcStage,
              quoll::rhi::PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(imageBarrier.dstStage, quoll::rhi::PipelineStage::FragmentShader);
    EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::ColorAttachmentWrite);
    EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ShaderRead);
    EXPECT_EQ(imageBarrier.srcLayout,
              quoll::rhi::ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(imageBarrier.dstLayout,
              quoll::rhi::ImageLayout::ShaderReadOnlyOptimal);
  }
}

TEST_F(RenderGraphTest,
       SetsBufferBarrierFromUndefinedBeforeFirstPassThatWritesToTheBuffer) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, quoll::rhi::BufferUsage::Storage);
  }

  graph.build(storage);

  auto dependencies = graph.getCompiledPasses().at(0).getSyncDependencies();
  EXPECT_EQ(dependencies.bufferBarriers.size(), 1);

  const auto &bufferBarrier = dependencies.bufferBarriers.at(0);
  EXPECT_EQ(bufferBarrier.buffer, buffer1);
  EXPECT_EQ(bufferBarrier.srcStage, quoll::rhi::PipelineStage::None);
  EXPECT_EQ(bufferBarrier.dstStage, quoll::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(bufferBarrier.srcAccess, quoll::rhi::Access::None);
  EXPECT_EQ(bufferBarrier.dstAccess, quoll::rhi::Access::ShaderWrite);
}

TEST_F(RenderGraphTest, SetsBufferBarrierBetweenBufferWrites) {
  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, quoll::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addComputePass("C");
    pass.write(buffer1, quoll::rhi::BufferUsage::Uniform);
  }

  graph.build(storage);

  auto dependencies = graph.getCompiledPasses().at(1).getSyncDependencies();
  EXPECT_EQ(dependencies.bufferBarriers.size(), 1);

  const auto &bufferBarrier = dependencies.bufferBarriers.at(0);
  EXPECT_EQ(bufferBarrier.buffer, buffer1);
  EXPECT_EQ(bufferBarrier.srcStage, quoll::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(bufferBarrier.dstStage, quoll::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(bufferBarrier.srcAccess, quoll::rhi::Access::ShaderWrite);
  EXPECT_EQ(bufferBarrier.dstAccess, quoll::rhi::Access::ShaderWrite);
  EXPECT_EQ(bufferBarrier.size, 0);
  EXPECT_EQ(bufferBarrier.offset, 0);
}

TEST_F(RenderGraphTest, SetsBufferBarrierBetweenMultipleBufferWrites) {
  auto buffer1 = device.createBuffer({}).getHandle();
  auto buffer2 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, quoll::rhi::BufferUsage::Storage);
    pass.write(buffer2, quoll::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addComputePass("C");
    pass.write(buffer1, quoll::rhi::BufferUsage::Uniform);
    pass.write(buffer2, quoll::rhi::BufferUsage::Storage);
  }

  graph.build(storage);

  auto dependencies = graph.getCompiledPasses().at(1).getSyncDependencies();
  EXPECT_EQ(dependencies.bufferBarriers.size(), 2);

  {
    const auto &bufferBarrier = dependencies.bufferBarriers.at(0);
    EXPECT_EQ(bufferBarrier.buffer, buffer1);
    EXPECT_EQ(bufferBarrier.srcStage,
              quoll::rhi::PipelineStage::FragmentShader);
    EXPECT_EQ(bufferBarrier.dstStage, quoll::rhi::PipelineStage::ComputeShader);
    EXPECT_EQ(bufferBarrier.srcAccess, quoll::rhi::Access::ShaderWrite);
    EXPECT_EQ(bufferBarrier.dstAccess, quoll::rhi::Access::ShaderWrite);
    EXPECT_EQ(bufferBarrier.size, 0);
    EXPECT_EQ(bufferBarrier.offset, 0);
  }

  {
    const auto &bufferBarrier = dependencies.bufferBarriers.at(1);
    EXPECT_EQ(bufferBarrier.buffer, buffer2);
    EXPECT_EQ(bufferBarrier.srcStage,
              quoll::rhi::PipelineStage::FragmentShader);
    EXPECT_EQ(bufferBarrier.dstStage, quoll::rhi::PipelineStage::ComputeShader);
    EXPECT_EQ(bufferBarrier.srcAccess, quoll::rhi::Access::ShaderWrite);
    EXPECT_EQ(bufferBarrier.dstAccess, quoll::rhi::Access::ShaderWrite);
    EXPECT_EQ(bufferBarrier.size, 0);
    EXPECT_EQ(bufferBarrier.offset, 0);
  }
}

TEST_F(RenderGraphTest, SetsBufferBarrierBetweenBufferWriteAndRead) {
  auto buffer1 = device.createBuffer({}).getHandle();
  auto buffer2 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, quoll::rhi::BufferUsage::Storage);
  }

  {
    auto &pass = graph.addComputePass("C");
    pass.read(buffer1, quoll::rhi::BufferUsage::Uniform);
  }

  graph.build(storage);

  auto dependencies = graph.getCompiledPasses().at(1).getSyncDependencies();
  EXPECT_EQ(dependencies.bufferBarriers.size(), 1);

  const auto &bufferBarrier = dependencies.bufferBarriers.at(0);
  EXPECT_EQ(bufferBarrier.buffer, buffer1);
  EXPECT_EQ(bufferBarrier.srcStage, quoll::rhi::PipelineStage::FragmentShader);
  EXPECT_EQ(bufferBarrier.dstStage, quoll::rhi::PipelineStage::ComputeShader);
  EXPECT_EQ(bufferBarrier.srcAccess, quoll::rhi::Access::ShaderWrite);
  EXPECT_EQ(bufferBarrier.dstAccess, quoll::rhi::Access::ShaderRead);
  EXPECT_EQ(bufferBarrier.size, 0);
  EXPECT_EQ(bufferBarrier.offset, 0);
}

TEST_F(RenderGraphTest, SetsBufferAndTextureBarriersBetweenPasses) {
  quoll::rhi::TextureDescription colorDescription{};
  colorDescription.usage = quoll::rhi::TextureUsage::Color;
  auto colorTexture = createTexture(colorDescription);

  auto buffer1 = device.createBuffer({}).getHandle();

  {
    auto &pass = graph.addGraphicsPass("A");
    pass.write(buffer1, quoll::rhi::BufferUsage::Storage);
    pass.write(colorTexture, quoll::AttachmentType::Color, {});
  }

  {
    auto &pass = graph.addComputePass("C");
    pass.read(buffer1, quoll::rhi::BufferUsage::Uniform);
    pass.write(colorTexture, quoll::AttachmentType::Color, {});
  }

  graph.build(storage);

  auto dependencies = graph.getCompiledPasses().at(1).getSyncDependencies();
  EXPECT_EQ(dependencies.imageBarriers.size(), 1);
  EXPECT_EQ(dependencies.bufferBarriers.size(), 1);

  {
    const auto &imageBarrier = dependencies.imageBarriers.at(0);
    EXPECT_EQ(imageBarrier.texture, colorTexture);
    EXPECT_EQ(imageBarrier.srcStage,
              quoll::rhi::PipelineStage::ColorAttachmentOutput);
    EXPECT_EQ(imageBarrier.dstStage, quoll::rhi::PipelineStage::ComputeShader);
    EXPECT_EQ(imageBarrier.srcAccess, quoll::rhi::Access::ColorAttachmentWrite);
    EXPECT_EQ(imageBarrier.dstAccess, quoll::rhi::Access::ShaderWrite);
    EXPECT_EQ(imageBarrier.srcLayout,
              quoll::rhi::ImageLayout::ColorAttachmentOptimal);
    EXPECT_EQ(imageBarrier.dstLayout, quoll::rhi::ImageLayout::General);
  }

  {
    const auto &bufferBarrier = dependencies.bufferBarriers.at(0);
    EXPECT_EQ(bufferBarrier.buffer, buffer1);
    EXPECT_EQ(bufferBarrier.srcStage,
              quoll::rhi::PipelineStage::FragmentShader);
    EXPECT_EQ(bufferBarrier.dstStage, quoll::rhi::PipelineStage::ComputeShader);
    EXPECT_EQ(bufferBarrier.srcAccess, quoll::rhi::Access::ShaderWrite);
    EXPECT_EQ(bufferBarrier.dstAccess, quoll::rhi::Access::ShaderRead);
    EXPECT_EQ(bufferBarrier.size, 0);
    EXPECT_EQ(bufferBarrier.offset, 0);
  }
}

TEST_F(RenderGraphTest, BuildsRenderPassWithOnlyColorAttachments) {
  TextureDescription colorDescription{};
  colorDescription.usage = TextureUsage::Color;
  colorDescription.width = 1024;
  colorDescription.height = 768;
  colorDescription.layerCount = 10;
  auto texture = createTexture(colorDescription);

  auto &pass = graph.addGraphicsPass("A");
  pass.write(texture, quoll::AttachmentType::Color, glm::vec4{2.5f});

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  EXPECT_NE(compiled.getRenderPass(), RenderPassHandle::Null);
  EXPECT_NE(compiled.getFramebuffer(), FramebufferHandle::Null);

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
  colorDescription.layerCount = 10;
  auto texture = createTexture(colorDescription);

  auto &pass = graph.addGraphicsPass("A");
  pass.write(texture, quoll::AttachmentType::Depth,
             DepthStencilClear{2.5f, 35});

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  EXPECT_NE(compiled.getRenderPass(), RenderPassHandle::Null);
  EXPECT_NE(compiled.getFramebuffer(), FramebufferHandle::Null);

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
  colorDescription.layerCount = 10;
  auto texture = createTexture(colorDescription);

  auto &pass = graph.addGraphicsPass("A");
  pass.write(texture, quoll::AttachmentType::Resolve, glm::vec4{2.5f});

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  EXPECT_NE(compiled.getRenderPass(), RenderPassHandle::Null);
  EXPECT_NE(compiled.getFramebuffer(), FramebufferHandle::Null);

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
  description.layerCount = 10;
  auto color1 = createTexture(description);
  auto color2 = createTexture(description);
  auto depth = createTexture(description);
  auto resolve = createTexture(description);

  auto &pass = graph.addGraphicsPass("A");
  pass.write(color1, quoll::AttachmentType::Color, glm::vec4{2.5f});
  pass.write(color2, quoll::AttachmentType::Color, glm::vec4{4.5f});
  pass.write(depth, quoll::AttachmentType::Depth, DepthStencilClear{2.5f, 35});
  pass.write(resolve, quoll::AttachmentType::Resolve, glm::vec4{5.5f});

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
  pass.write(texture, quoll::AttachmentType::Color, glm::vec4{0.0f});
  pass.addPipeline(pipeline);

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  EXPECT_NE(compiled.getRenderPass(), RenderPassHandle::Null);
  EXPECT_NE(compiled.getFramebuffer(), FramebufferHandle::Null);

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
  pass.write(texture, quoll::AttachmentType::Color, glm::vec4{0.0f});
  pass.addPipeline(pipeline);

  graph.build(storage);

  auto &compiled = graph.getCompiledPasses().at(0);

  auto mock = device.getPipeline(pipeline);
  EXPECT_EQ(mock.getType(), MockPipeline::Type::Compute);
  auto mockDescription = mock.getComputeDecription();

  EXPECT_EQ(mockDescription.computeShader, ShaderHandle{25});
}

TEST_F(RenderGraphDeathTest, FailsIfPassReadsFromNonWrittenTexture) {
  using TextureDescription = quoll::rhi::TextureDescription;
  using TextureUsage = quoll::rhi::TextureUsage;
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

  graph.addGraphicsPass("A").write(handle, quoll::AttachmentType::Color,
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

  graph.addGraphicsPass("A").write(handle, quoll::AttachmentType::Color,
                                   glm::vec4());
  graph.addGraphicsPass("E").read(handle);

  graph.build(storage);
  EXPECT_EQ(graph.getCompiledPasses().size(), 2);

  graph.build(storage);
  EXPECT_EQ(graph.getCompiledPasses().size(), 2);
}

TEST_F(RenderGraphDeathTest, CompilationFailsIfMultipleNodesHaveTheSameName) {
  auto handle = createTexture({});

  graph.addGraphicsPass("A").write(handle, quoll::AttachmentType::Color,
                                   glm::vec4());
  graph.addGraphicsPass("B");
  graph.addGraphicsPass("A");
  graph.addGraphicsPass("E");

  EXPECT_DEATH(graph.build(storage), ".*");
}

TEST_F(RenderGraphTest, DestroysAllTransientResourcesOnDestroy) {
  TextureDescription fixedDescription{};
  fixedDescription.width = 1920;
  fixedDescription.height = 1080;

  auto t1 = graph.create(fixedDescription);
  auto t2 = graph.createView(t1, 1, 2, 3, 4);

  auto pipeline =
      storage.addPipeline(quoll::rhi::GraphicsPipelineDescription{});

  auto &pass = graph.addGraphicsPass("test");
  pass.write(t1, quoll::AttachmentType::Color, {});
  pass.addPipeline(pipeline);

  graph.build(storage);

  auto &cpass = graph.getCompiledPasses().at(0);

  EXPECT_TRUE(device.hasTexture(t1.getHandle()));
  EXPECT_TRUE(device.hasTexture(t2.getHandle()));
  EXPECT_TRUE(device.hasFramebuffer(cpass.getFramebuffer()));
  EXPECT_TRUE(device.hasRenderPass(cpass.getRenderPass()));
  EXPECT_TRUE(device.hasPipeline(pipeline));

  graph.destroy(storage);

  EXPECT_FALSE(device.hasTexture(t1.getHandle()));
  EXPECT_FALSE(device.hasTexture(t2.getHandle()));
  EXPECT_FALSE(device.hasFramebuffer(cpass.getFramebuffer()));
  EXPECT_FALSE(device.hasRenderPass(cpass.getRenderPass()));
  EXPECT_FALSE(device.hasPipeline(pipeline));
}

TEST_F(RenderGraphTest, DoesNotDestroyImportedResourcesOnDestroy) {
  auto texture = storage.createTexture({});

  graph.import(texture);

  graph.build(storage);

  EXPECT_TRUE(device.hasTexture(texture));

  graph.destroy(storage);

  EXPECT_TRUE(device.hasTexture(texture));
}
