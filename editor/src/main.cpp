#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/renderer/Material.h"

#include "liquid/renderer/Renderer.h"
#include "liquid/scene/Vertex.h"
#include "liquid/scene/Mesh.h"
#include "liquid/scene/MeshInstance.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/window/Window.h"
#include "liquid/renderer/StandardPushConstants.h"
#include "liquid/profiler/ImguiDebugLayer.h"

#include "liquid/rhi/vulkan/VulkanRenderBackend.h"

#include "liquid/loaders/GLTFLoader.h"
#include "liquid/loaders/ImageTextureLoader.h"

#include "liquid/physics/PhysicsSystem.h"

#include "liquid/loop/MainLoop.h"

#include "editor-scene/EditorCamera.h"
#include "editor-scene/SceneManager.h"
#include "editor-scene/EditorGrid.h"
#include "ui/UIRoot.h"

static const uint32_t INITIAL_WIDTH = 1024;
static const uint32_t INITIAL_HEIGHT = 768;

int main() {
  liquid::Engine::setAssetsPath(
      std::filesystem::path("../../../engine/bin/Debug/assets").string());
  liquid::EntityContext entityContext;
  liquid::Window window("Liquidator", INITIAL_WIDTH, INITIAL_HEIGHT);

  liquid::rhi::VulkanRenderBackend backend(window);
  liquid::DebugManager debugManager;

  auto *device = backend.getOrCreateDevice();

  liquid::Renderer renderer(entityContext, window, device);
  liquid::AnimationSystem animationSystem(entityContext);
  liquid::PhysicsSystem physicsSystem(entityContext);

  liquid::ImguiDebugLayer debugLayer(
      device->getPhysicalDevice().getDeviceInfo(), renderer.getStatsManager(),
      debugManager);

  renderer.getShaderLibrary().addShader(
      "editor-grid.vert", renderer.getRegistry().setShader(
                              {"assets/shaders/editor-grid.vert.spv"}));
  renderer.getShaderLibrary().addShader(
      "editor-grid.frag", renderer.getRegistry().setShader(
                              {"assets/shaders/editor-grid.frag.spv"}));

  renderer.getShaderLibrary().addShader(
      "skeleton-lines.vert", renderer.getRegistry().setShader(
                                 {"assets/shaders/skeleton-lines.vert.spv"}));
  renderer.getShaderLibrary().addShader(
      "skeleton-lines.frag", renderer.getRegistry().setShader(
                                 {"assets/shaders/skeleton-lines.frag.spv"}));

  liquid::MainLoop mainLoop(window, renderer.getStatsManager());
  liquid::GLTFLoader loader(entityContext, renderer, animationSystem, true);
  liquidator::EditorCamera editorCamera(entityContext, renderer, window);
  liquidator::EditorGrid editorGrid(renderer.getRegistry());
  liquidator::SceneManager sceneManager(entityContext, editorCamera,
                                        editorGrid);

  liquidator::UIRoot ui(entityContext, loader);

  while (sceneManager.hasNewScene()) {
    sceneManager.createNewScene();

    const auto &cameraObj =
        entityContext
            .getComponent<liquid::CameraComponent>(editorCamera.getCamera())
            .camera;

    const auto &renderData =
        renderer.prepareScene(sceneManager.getActiveScene());

    liquid::RenderGraph graph =
        renderer.createRenderGraph(renderData, "mainColor");

    struct EditorDebugScope {
      liquid::GraphResourceId editorGridPipeline = 0;
      liquid::GraphResourceId skeletonLinesPipeline = 0;
    };

    graph.addInlinePass<EditorDebugScope>(
        "editorDebug",
        [&renderer](liquid::RenderGraphBuilder &builder,
                    EditorDebugScope &scope) {
          builder.write("mainColor");
          builder.write("depthBuffer");

          scope.editorGridPipeline =
              builder.create(liquid::RenderGraphPipelineDescription{
                  renderer.getShaderLibrary().getShader("editor-grid.vert"),
                  renderer.getShaderLibrary().getShader("editor-grid.frag"),
                  {},
                  liquid::PipelineInputAssembly{},
                  liquid::PipelineRasterizer{liquid::PolygonMode::Fill,
                                             liquid::CullMode::None,
                                             liquid::FrontFace::Clockwise},
                  liquid::PipelineColorBlend{
                      {liquid::PipelineColorBlendAttachment{
                          true, liquid::BlendFactor::SrcAlpha,
                          liquid::BlendFactor::DstAlpha, liquid::BlendOp::Add,
                          liquid::BlendFactor::SrcAlpha,
                          liquid::BlendFactor::DstAlpha,
                          liquid::BlendOp::Add}}}});

          scope.skeletonLinesPipeline =
              builder.create(liquid::RenderGraphPipelineDescription{
                  renderer.getShaderLibrary().getShader("skeleton-lines.vert"),
                  renderer.getShaderLibrary().getShader("skeleton-lines.frag"),
                  {},
                  liquid::PipelineInputAssembly{
                      liquid::PrimitiveTopology::LineList},
                  liquid::PipelineRasterizer{liquid::PolygonMode::Line,
                                             liquid::CullMode::None,
                                             liquid::FrontFace::Clockwise},
                  liquid::PipelineColorBlend{
                      {liquid::PipelineColorBlendAttachment{
                          true, liquid::BlendFactor::SrcAlpha,
                          liquid::BlendFactor::DstAlpha, liquid::BlendOp::Add,
                          liquid::BlendFactor::SrcAlpha,
                          liquid::BlendFactor::DstAlpha,
                          liquid::BlendOp::Add}}}});
        },
        [&renderer, &cameraObj, &editorCamera, &editorGrid, &entityContext](
            liquid::rhi::RenderCommandList &commandList,
            EditorDebugScope &scope, liquid::RenderGraphRegistry &registry) {
          const auto &pipeline = registry.getPipeline(scope.editorGridPipeline);

          liquid::rhi::Descriptor sceneDescriptor;
          sceneDescriptor.bind(0, cameraObj->getBuffer(),
                               liquid::rhi::DescriptorType::UniformBuffer);

          liquid::rhi::Descriptor gridDescriptor;
          gridDescriptor.bind(0, editorGrid.getBuffer(),
                              liquid::rhi::DescriptorType::UniformBuffer);

          commandList.bindPipeline(pipeline);
          commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
          commandList.bindDescriptor(pipeline, 1, gridDescriptor);

          constexpr uint32_t PLANE_VERTICES = 6;
          commandList.draw(PLANE_VERTICES, 0);

          const auto &skeletonPipeline =
              registry.getPipeline(scope.skeletonLinesPipeline);
          commandList.bindPipeline(skeletonPipeline);

          entityContext.iterateEntities<liquid::TransformComponent,
                                        liquid::SkeletonComponent,
                                        liquid::DebugComponent>(
              [&commandList, &skeletonPipeline,
               &cameraObj](auto entity, auto &transform,
                           const liquid::SkeletonComponent &skeleton,
                           const liquid::DebugComponent &debug) {
                if (!debug.showBones)
                  return;

                liquid::rhi::Descriptor sceneDescriptor;
                sceneDescriptor.bind(
                    0, cameraObj->getBuffer(),
                    liquid::rhi::DescriptorType::UniformBuffer);

                liquid::rhi::Descriptor skeletonDescriptor;
                skeletonDescriptor.bind(
                    0, skeleton.skeleton.getDebugBuffer(),
                    liquid::rhi::DescriptorType::UniformBuffer);

                auto *transformConstant = new liquid::StandardPushConstants;
                transformConstant->modelMatrix = transform.worldTransform;

                commandList.bindDescriptor(skeletonPipeline, 0,
                                           sceneDescriptor);
                commandList.bindDescriptor(skeletonPipeline, 1,
                                           skeletonDescriptor);

                commandList.pushConstants(
                    skeletonPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                    sizeof(liquid::StandardPushConstants), transformConstant);

                commandList.draw(skeleton.skeleton.getNumDebugBones(), 0);
              });
        });

    mainLoop.setUpdateFn([&editorCamera, &sceneManager, &renderData,
                          &animationSystem, &physicsSystem,
                          &entityContext](double dt) mutable {
      editorCamera.update();

      animationSystem.update(static_cast<float>(dt));
      renderData->update();

      entityContext.iterateEntities<liquid::SkeletonComponent>(
          [](auto entity, auto &component) {
            component.skeleton.updateDebug();
          });

      sceneManager.getActiveScene()->update();
      physicsSystem.update(static_cast<float>(dt));
      return !sceneManager.hasNewScene();
    });

    mainLoop.setRenderFn([&renderer, &graph, &ui, &sceneManager,
                          &animationSystem, &physicsSystem, &debugLayer]() {
      auto &imgui = renderer.getImguiRenderer();

      imgui.beginRendering();
      ui.render(sceneManager, animationSystem, physicsSystem);

      if (ImGui::Begin("View")) {
        const auto &size = ImGui::GetContentRegionAvail();
        const auto &pos = ImGui::GetWindowPos();
        sceneManager.getEditorCamera().setViewport(pos.x, pos.y, size.x,
                                                   size.y);
        ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(
                         graph.getResourceId("mainColor"))),
                     size);
        ImGui::End();
      }

      debugLayer.render();
      imgui.endRendering();

      return renderer.render(graph);
    });

    mainLoop.run();
  }

  renderer.wait();
  return 0;
}
