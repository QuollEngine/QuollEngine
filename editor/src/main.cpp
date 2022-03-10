#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/renderer/Material.h"
#include "liquid/renderer/Shader.h"

#include "liquid/renderer/vulkan/VulkanRenderer.h"
#include "liquid/scene/Vertex.h"
#include "liquid/scene/Mesh.h"
#include "liquid/scene/MeshInstance.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/window/glfw/GLFWWindow.h"
#include "liquid/renderer/vulkan/VulkanStandardPushConstants.h"

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
  liquid::EntityContext context;
  std::unique_ptr<liquid::GLFWWindow> window(
      new liquid::GLFWWindow("Liquidator", INITIAL_WIDTH, INITIAL_HEIGHT));

  liquid::experimental::VulkanRenderBackend backend(*window.get());

  std::unique_ptr<liquid::VulkanRenderer> renderer(new liquid::VulkanRenderer(
      context, window.get(), backend.getOrCreateDevice()));
  liquid::AnimationSystem animationSystem(context);
  liquid::PhysicsSystem physicsSystem(context);

  renderer->getShaderLibrary()->addShader(
      "editor-grid.vert",
      renderer->createShader("assets/shaders/editor-grid.vert.spv"));
  renderer->getShaderLibrary()->addShader(
      "editor-grid.frag",
      renderer->createShader("assets/shaders/editor-grid.frag.spv"));

  renderer->getShaderLibrary()->addShader(
      "skeleton-lines.vert",
      renderer->createShader("assets/shaders/skeleton-lines.vert.spv"));
  renderer->getShaderLibrary()->addShader(
      "skeleton-lines.frag",
      renderer->createShader("assets/shaders/skeleton-lines.frag.spv"));

  liquid::MainLoop mainLoop(renderer.get(), window.get());
  liquid::GLTFLoader loader(context, renderer.get(), animationSystem, true);
  liquidator::EditorCamera editorCamera(context, renderer.get(), window.get());
  liquidator::EditorGrid editorGrid(renderer->getRenderBackend().getRegistry());
  liquidator::SceneManager sceneManager(context, editorCamera, editorGrid);

  liquidator::UIRoot ui(context, loader);

  while (sceneManager.hasNewScene()) {
    sceneManager.createNewScene();

    const auto &cameraObj =
        context.getComponent<liquid::CameraComponent>(editorCamera.getCamera())
            .camera;

    const auto &renderData =
        renderer->prepareScene(sceneManager.getActiveScene());

    liquid::RenderGraph graph = renderer->createRenderGraph(
        renderData, "mainColor",
        [&sceneManager, &animationSystem, &ui, &renderData,
         &physicsSystem](const auto &sceneTexture) {
          ui.render(sceneManager, animationSystem, physicsSystem);
          if (ImGui::Begin("View")) {
            const auto &size = ImGui::GetContentRegionAvail();
            const auto &pos = ImGui::GetWindowPos();
            sceneManager.getEditorCamera().setViewport(pos.x, pos.y, size.x,
                                                       size.y);
            ImGui::Image(
                reinterpret_cast<void *>(static_cast<uintptr_t>(sceneTexture)),
                size);
            ImGui::End();
          }
        });

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

          scope.editorGridPipeline = builder.create(liquid::PipelineDescriptor{
              renderer->getShaderLibrary()->getShader("editor-grid.vert"),
              renderer->getShaderLibrary()->getShader("editor-grid.frag"),
              {},
              liquid::PipelineInputAssembly{},
              liquid::PipelineRasterizer{liquid::PolygonMode::Fill,
                                         liquid::CullMode::None,
                                         liquid::FrontFace::Clockwise},
              liquid::PipelineColorBlend{{liquid::PipelineColorBlendAttachment{
                  true, liquid::BlendFactor::SrcAlpha,
                  liquid::BlendFactor::DstAlpha, liquid::BlendOp::Add,
                  liquid::BlendFactor::SrcAlpha, liquid::BlendFactor::DstAlpha,
                  liquid::BlendOp::Add}}}});

          scope.skeletonLinesPipeline =
              builder.create(liquid::PipelineDescriptor{
                  renderer->getShaderLibrary()->getShader(
                      "skeleton-lines.vert"),
                  renderer->getShaderLibrary()->getShader(
                      "skeleton-lines.frag"),
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
        [&renderer, &cameraObj, &editorCamera, &editorGrid, &context](
            liquid::RenderCommandList &commandList, EditorDebugScope &scope,
            liquid::RenderGraphRegistry &registry) {
          const auto &pipeline = registry.getPipeline(scope.editorGridPipeline);

          liquid::Descriptor sceneDescriptor;
          sceneDescriptor.bind(0, cameraObj->getUniformBuffer(),
                               liquid::DescriptorType::UniformBuffer);

          liquid::Descriptor gridDescriptor;
          gridDescriptor.bind(0, editorGrid.getUniformBuffer(),
                              liquid::DescriptorType::UniformBuffer);

          commandList.bindPipeline(pipeline);
          commandList.bindDescriptor(pipeline, 0, sceneDescriptor);
          commandList.bindDescriptor(pipeline, 1, gridDescriptor);

          constexpr uint32_t PLANE_VERTICES = 6;
          commandList.draw(PLANE_VERTICES, 0);

          const auto &skeletonPipeline =
              registry.getPipeline(scope.skeletonLinesPipeline);
          commandList.bindPipeline(skeletonPipeline);

          context.iterateEntities<liquid::TransformComponent,
                                  liquid::SkeletonComponent,
                                  liquid::DebugComponent>(
              [&commandList, &skeletonPipeline,
               &cameraObj](auto entity, auto &transform,
                           const liquid::SkeletonComponent &skeleton,
                           const liquid::DebugComponent &debug) {
                if (!debug.showBones)
                  return;

                liquid::Descriptor sceneDescriptor;
                sceneDescriptor.bind(0, cameraObj->getUniformBuffer(),
                                     liquid::DescriptorType::UniformBuffer);

                liquid::Descriptor skeletonDescriptor;
                skeletonDescriptor.bind(0, skeleton.skeleton.getDebugBuffer(),
                                        liquid::DescriptorType::UniformBuffer);

                auto *transformConstant =
                    new liquid::VulkanStandardPushConstants;
                transformConstant->modelMatrix = transform.worldTransform;

                commandList.bindDescriptor(skeletonPipeline, 0,
                                           sceneDescriptor);
                commandList.bindDescriptor(skeletonPipeline, 1,
                                           skeletonDescriptor);

                commandList.pushConstants(
                    skeletonPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                    sizeof(liquid::VulkanStandardPushConstants),
                    transformConstant);

                commandList.draw(skeleton.skeleton.getNumDebugBones(), 0);
              });
        });

    mainLoop.run(graph,
                 [&editorCamera, &sceneManager, &renderData, &animationSystem,
                  &physicsSystem, &context](double dt) mutable {
                   editorCamera.update();

                   animationSystem.update(static_cast<float>(dt));
                   renderData->update();

                   context.iterateEntities<liquid::SkeletonComponent>(
                       [](auto entity, auto &component) {
                         component.skeleton.updateDebug();
                       });

                   sceneManager.getActiveScene()->update();
                   physicsSystem.update(static_cast<float>(dt));
                   return !sceneManager.hasNewScene();
                 });
  }

  return 0;
}
