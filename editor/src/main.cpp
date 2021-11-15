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

#include "loaders/TinyGLTFLoader.h"
#include "loaders/ImageTextureLoader.h"

#include "loop/MainLoop.h"

#include "editor-scene/EditorCamera.h"
#include "ui/MenuBar.h"
#include "ui/SceneHierarchyPanel.h"

static const glm::vec4 CLEAR_COLOR{0.19, 0.21, 0.26, 1.0};
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
        new liquid::VulkanRenderer(context, window.get()));

    liquid::MainLoop mainLoop(renderer.get(), window.get());
    liquid::TinyGLTFLoader loader(context, renderer.get());

    liquidator::EditorCamera editorCamera(renderer.get(), window.get());

    const auto &scene = std::make_shared<liquid::Scene>(context);

    scene->setActiveCamera(editorCamera.getCamera().get());

    scene->getRootNode()->addChild(loader.loadFromFile("default-scene.gltf"));

    auto light1 = context.createEntity();
    context.setComponent<liquid::NameComponent>(light1, {"Light"});
    context.setComponent<liquid::LightComponent>(
        light1, {std::make_shared<liquid::Light>(
                    liquid::Light::DIRECTIONAL, glm::vec3{0.0f, 1.0f, 1.0f},
                    glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 1.0f)});
    scene->getRootNode()->addChild(light1);

    renderer->setClearColor(CLEAR_COLOR);

    liquidator::MenuBar menuBar(loader);
    liquidator::SceneHierarchyPanel sceneHierarchyPanel(context);

    return mainLoop.run(
        scene.get(),
        [&editorCamera](double dt) mutable {
          ImGuiIO &io = ImGui::GetIO();
          if (!io.WantCaptureMouse && !io.WantCaptureKeyboard) {
            editorCamera.update();
          }
          return true;
        },
        [&sceneHierarchyPanel, &menuBar, &scene]() {
          menuBar.render(scene.get());
          sceneHierarchyPanel.render(scene.get());
        });
  } catch (std::runtime_error error) {
    std::cerr << error.what() << std::endl;
    return 1;
  }
}
