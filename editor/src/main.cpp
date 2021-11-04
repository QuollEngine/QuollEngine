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
#include "scene/gltf/TinyGLTFLoader.h"

#include "loaders/ImageTextureLoader.h"

#include "loop/MainLoop.h"

#include "editor-scene/EditorCamera.h"
#include "ui/MenuBar.h"

int main() {
  try {
    liquid::Engine::setAssetsPath(
        std::filesystem::path("../../../engine/bin/Debug/assets").string());
    liquid::EntityContext context;
    std::unique_ptr<liquid::GLFWWindow> window(
        new liquid::GLFWWindow("Liquidator", 1024, 768));
    std::unique_ptr<liquid::VulkanRenderer> renderer(
        new liquid::VulkanRenderer(context, window.get()));

    liquidator::EditorCamera editorCamera(renderer.get(), window.get());

    liquid::MainLoop mainLoop(renderer.get(), window.get());

    liquid::TinyGLTFLoader loader(context, renderer.get());
    const auto &scene = loader.loadFromFile("default-scene.gltf");
    scene->setActiveCamera(editorCamera.getCamera().get());

    liquidator::MenuBar menuBar;

    auto light1 = context.createEntity();

    context.setComponent<liquid::LightComponent>(
        light1, {std::make_shared<liquid::Light>(
                    liquid::Light::DIRECTIONAL, glm::vec3{0.0f, 1.0f, 1.0f},
                    glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 1.0f)});

    auto *node = scene->getRootNode();
    node->addChild(light1);

    renderer->setClearColor({0.19, 0.21, 0.26, 1.0});

    return mainLoop.run(
        scene.get(),
        [&editorCamera](double dt) mutable {
          ImGuiIO &io = ImGui::GetIO();
          if (!io.WantCaptureMouse && !io.WantCaptureKeyboard) {
            editorCamera.update();
          }
          return true;
        },
        [&menuBar]() {
          // menuBar.render();
        });
  } catch (std::runtime_error error) {
    std::cerr << error.what() << std::endl;
    return 1;
  }
}
