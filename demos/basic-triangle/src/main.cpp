#include "core/Base.h"
#include "core/Engine.h"

#include "renderer/Material.h"
#include "renderer/Texture.h"
#include "renderer/Shader.h"

#include "renderer/vulkan/VulkanRenderer.h"
#include "scene/Vertex.h"
#include "scene/Mesh.h"
#include "scene/MeshInstance.h"
#include "entity/EntityContext.h"
#include "window/glfw/GLFWWindow.h"
#include "renderer/passes/ImguiPass.h"

#include "loaders/ImageTextureLoader.h"

#include "loop/MainLoop.h"

#include <GLFW/glfw3.h>

uint32_t materialIndex = 0;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    materialIndex++;
  }
}

int main() {
  liquid::Engine::setAssetsPath(
      std::filesystem::path("../../../../engine/bin/Debug/assets").string());
  liquid::EntityContext context;
  std::unique_ptr<liquid::GLFWWindow> window(
      new liquid::GLFWWindow("Triangle", 640, 480));
  std::unique_ptr<liquid::VulkanRenderer> renderer(
      new liquid::VulkanRenderer(context, window.get()));
  auto entity = context.createEntity();

  liquid::ImageTextureLoader textureLoader(renderer->getResourceAllocator());

  auto &&shaderBasicVert = renderer->createShader("basic-shader.vert.spv");
  auto &&shaderBasicFrag = renderer->createShader("basic-shader.frag.spv");
  auto &&shaderRedFrag = renderer->createShader("red-shader.frag.spv");
  auto &&shaderTextureVert = renderer->createShader("texture-shader.vert.spv");
  auto &&shaderTextureFrag = renderer->createShader("texture-shader.frag.spv");

  std::array<liquid::SharedPtr<liquid::Material>, 3> materials{
      renderer->createMaterial(shaderBasicVert, shaderBasicFrag, {}, {},
                               liquid::CullMode::None),
      renderer->createMaterial(shaderBasicVert, shaderRedFrag, {}, {},
                               liquid::CullMode::None),
      renderer->createMaterial(shaderTextureVert, shaderTextureFrag,
                               {textureLoader.loadFromFile("brick.png")}, {},
                               liquid::CullMode::None)};

  liquid::Mesh mesh;
  liquid::Geometry geom;
  geom.addVertex({-1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
                  0.0, 0.0, 0.0});
  geom.addVertex({0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                  0.0, 0.0, 1.0});
  geom.addVertex({1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                  1.0, 1.0, 1.0});

  geom.addTriangle(0, 1, 2);
  geom.setMaterial(materials[materialIndex % 3]);
  mesh.addGeometry(geom);

  liquid::SharedPtr<liquid::MeshInstance> instance(
      new liquid::MeshInstance(&mesh, renderer->getResourceAllocator()));
  context.setComponent<liquid::MeshComponent>(entity, {instance});

  std::unique_ptr<liquid::Scene> scene(new liquid::Scene(context));
  auto camera = context.createEntity();
  context.setComponent<liquid::CameraComponent>(
      camera,
      {std::make_shared<liquid::Camera>(renderer->getResourceAllocator())});

  scene->setActiveCamera(camera);
  scene->getRootNode()->addChild(entity, glm::mat4{1.0f});

  glfwSetKeyCallback(window->getInstance(), key_callback);

  liquid::MainLoop mainLoop(renderer.get(), window.get());

  auto *instancePtr = instance.get();

  struct Scope {
    std::array<liquid::GraphResourceId, 2> basicPipeline;
    std::array<liquid::GraphResourceId, 2> redPipeline;
    std::array<liquid::GraphResourceId, 2> texturePipeline;
  };

  liquid::RenderGraph graph;

  graph.create("depthBuffer",
               {liquid::AttachmentType::Depth,
                liquid::AttachmentSizeMethod::SwapchainRelative, 100, 100, 1,
                VK_FORMAT_D32_SFLOAT, liquid::DepthStencilClear{1.0f, 0}});
  graph.addInlinePass<Scope>(
      "mainPass",
      [shaderBasicVert, shaderBasicFrag, shaderRedFrag, shaderTextureVert,
       shaderTextureFrag,
       &materials](liquid::RenderGraphBuilder &builder, Scope &scope) {
        builder.write("SWAPCHAIN");

        builder.write("depthBuffer");

        auto createTrianglePipelines =
            [&builder](const liquid::SharedPtr<liquid::Shader> &vertShader,
                       const liquid::SharedPtr<liquid::Shader> &fragShader)
            -> std::array<liquid::GraphResourceId, 2> {
          return {
              builder.create(liquid::PipelineDescriptor{
                  vertShader, fragShader,
                  liquid::PipelineVertexInputLayout::create<liquid::Vertex>(),
                  liquid::PipelineInputAssembly{},
                  liquid::PipelineRasterizer{liquid::PolygonMode::Fill},
                  liquid::PipelineColorBlend{
                      {liquid::PipelineColorBlendAttachment{}}}}),
              builder.create(liquid::PipelineDescriptor{
                  vertShader, fragShader,
                  liquid::PipelineVertexInputLayout::create<liquid::Vertex>(),
                  liquid::PipelineInputAssembly{},
                  liquid::PipelineRasterizer{liquid::PolygonMode::Line},
                  liquid::PipelineColorBlend{
                      {liquid::PipelineColorBlendAttachment{}}}})};
        };

        scope.basicPipeline =
            createTrianglePipelines(shaderBasicVert, shaderBasicFrag);
        scope.redPipeline =
            createTrianglePipelines(shaderBasicVert, shaderRedFrag);
        scope.texturePipeline =
            createTrianglePipelines(shaderTextureVert, shaderTextureFrag);
      },
      [&instance, &renderer,
       &materials](liquid::RenderCommandList &commandList, Scope &scope,
                   liquid::RenderGraphRegistry &registry) {
        constexpr uint32_t BASIC_MATERIAL = 0;
        constexpr uint32_t RED_MATERIAL = 1;
        constexpr uint32_t TEXTURE_MATERIAL = 2;
        uint32_t pIdx = renderer->getDebugManager()->getWireframeMode() ? 1 : 0;
        uint32_t material = materialIndex % materials.size();

        if (material == BASIC_MATERIAL) {
          const auto &pipeline =
              registry.getPipeline(scope.basicPipeline.at(pIdx));
          commandList.bindPipeline(pipeline);

        } else if (material == RED_MATERIAL) {
          const auto &pipeline =
              registry.getPipeline(scope.redPipeline.at(pIdx));
          commandList.bindPipeline(pipeline);
        } else if (material == TEXTURE_MATERIAL) {
          const auto &pipeline =
              registry.getPipeline(scope.texturePipeline.at(pIdx));
          commandList.bindPipeline(pipeline);

          commandList.bindDescriptor(pipeline, 0,
                                     materials.at(material)->getDescriptor());
        }

        commandList.bindVertexBuffer(instance->getVertexBuffers().at(0));
        commandList.bindIndexBuffer(instance->getIndexBuffers().at(0),
                                    VK_INDEX_TYPE_UINT32);
        commandList.draw(3, 0);
      });

  graph.addPass<liquid::ImguiPass>("imgui", renderer->getRenderBackend(),
                                   renderer->getShaderLibrary(),
                                   renderer->getDebugManager(), "SWAPCHAIN",
                                   [](const auto &sceneTexture) {});

  mainLoop.run(graph,
               [instancePtr, materials](double dt) mutable { return true; });

  context.destroy();
  return 0;
}
