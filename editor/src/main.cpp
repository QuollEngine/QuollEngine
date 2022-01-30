#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/renderer/Material.h"
#include "liquid/renderer/Texture.h"
#include "liquid/renderer/Shader.h"

#include "liquid/renderer/vulkan/VulkanRenderer.h"
#include "liquid/scene/Vertex.h"
#include "liquid/scene/Mesh.h"
#include "liquid/scene/MeshInstance.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/window/glfw/GLFWWindow.h"

#include "liquid/loaders/GLTFLoader.h"
#include "liquid/loaders/ImageTextureLoader.h"

#include "liquid/loop/MainLoop.h"

#include "editor-scene/EditorCamera.h"
#include "editor-scene/SceneManager.h"
#include "editor-scene/EditorGrid.h"
#include "ui/UIRoot.h"

static const uint32_t INITIAL_WIDTH = 1024;
static const uint32_t INITIAL_HEIGHT = 768;

int main() {
  try {
    liquid::Engine::setAssetsPath(
        std::filesystem::path("../../../engine/bin/Debug/assets").string());
    liquid::EntityContext context;
    std::unique_ptr<liquid::GLFWWindow> window(
        new liquid::GLFWWindow("Liquidator", INITIAL_WIDTH, INITIAL_HEIGHT));
    std::unique_ptr<liquid::VulkanRenderer> renderer(
        new liquid::VulkanRenderer(context, window.get(), true));
    liquid::AnimationSystem animationSystem(context);

    renderer->getShaderLibrary()->addShader(
        "editor-grid.vert",
        renderer->createShader("assets/shaders/editor-grid.vert.spv"));
    renderer->getShaderLibrary()->addShader(
        "editor-grid.frag",
        renderer->createShader("assets/shaders/editor-grid.frag.spv"));

    liquid::MainLoop mainLoop(renderer.get(), window.get());
    liquid::GLTFLoader loader(context, renderer.get(), animationSystem);
    liquidator::EditorCamera editorCamera(context, renderer.get(),
                                          window.get());
    liquidator::EditorGrid editorGrid(renderer->getResourceAllocator());
    liquidator::SceneManager sceneManager(context, editorCamera, editorGrid);

    liquidator::UIRoot ui(context, loader);

    liquid::ImageTextureLoader textureLoader(renderer->getResourceAllocator());

    while (sceneManager.hasNewScene()) {
      sceneManager.createNewScene();

      const auto &cameraObj =
          context
              .getComponent<liquid::CameraComponent>(editorCamera.getCamera())
              .camera;

      const auto &renderData =
          renderer->prepareScene(sceneManager.getActiveScene());

      liquid::RenderGraph graph = renderer->createRenderGraph(
          renderData, "mainColor",
          [&sceneManager, &ui, &renderData](const auto &sceneTexture) {
            ui.render(sceneManager);
            if (ImGui::Begin("View")) {
              const auto &size = ImGui::GetContentRegionAvail();
              const auto &pos = ImGui::GetWindowPos();
              sceneManager.getEditorCamera().setViewport(pos.x, pos.y, size.x,
                                                         size.y);
              ImGui::Image(sceneTexture.get(), size);
              ImGui::End();
            }
          });

      struct EditorDebugScope {
        liquid::GraphResourceId editorGridPipeline = 0;
      };

      graph.addInlinePass<EditorDebugScope>(
          "editorDebug",
          [&renderer](liquid::RenderGraphBuilder &builder,
                      EditorDebugScope &scope) {
            builder.write("mainColor");
            builder.write("depthBuffer");

            scope.editorGridPipeline =
                builder.create(liquid::PipelineDescriptor{
                    renderer->getShaderLibrary()->getShader("editor-grid.vert"),
                    renderer->getShaderLibrary()->getShader("editor-grid.frag"),
                    {},
                    liquid::PipelineInputAssembly{},
                    liquid::PipelineRasterizer{liquid::PolygonMode::Fill,
                                               liquid::CullMode::None,
                                               liquid::FrontFace::Clockwise},
                    liquid::PipelineColorBlend{
                        {liquid::PipelineColorBlendAttachment{
                            true, liquid::BlendFactor::SrcAlpha,
                            liquid::BlendFactor::OneMinusSrcAlpha,
                            liquid::BlendOp::Add, liquid::BlendFactor::One,
                            liquid::BlendFactor::OneMinusSrcAlpha,
                            liquid::BlendOp::Add}}}});
          },
          [&renderer, &cameraObj, &editorCamera, &editorGrid](
              liquid::RenderCommandList &commandList, EditorDebugScope &scope,
              liquid::RenderGraphRegistry &registry) {
            const auto &pipeline =
                registry.getPipeline(scope.editorGridPipeline);

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
          });

      mainLoop.run(graph, [&editorCamera, &sceneManager, &renderData,
                           &animationSystem](double dt) mutable {
        editorCamera.update();
        sceneManager.getActiveScene()->update();

        animationSystem.update(static_cast<float>(dt));
        renderData->update();
        return !sceneManager.hasNewScene();
      });
    }

    return 0;
  } catch (std::runtime_error error) {
    std::cerr << error.what() << std::endl;
    return 1;
  }
}
