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
#include "editor-scene/SceneManager.h"
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

    liquid::MainLoop mainLoop(renderer.get(), window.get());
    liquid::TinyGLTFLoader loader(context, renderer.get());
    liquidator::EditorCamera editorCamera(context, renderer.get(),
                                          window.get());
    liquidator::SceneManager sceneManager(context, editorCamera);

    liquidator::UIRoot ui(context, loader);

    liquid::ImageTextureLoader textureLoader(renderer->getResourceAllocator());

    while (sceneManager.hasNewScene()) {
      sceneManager.createNewScene();

      const auto &renderData =
          renderer->prepareScene(sceneManager.getActiveScene());

      liquid::RenderGraph graph = renderer->createRenderGraph(renderData);

      mainLoop.run(
          graph,
          [&editorCamera, &sceneManager](double dt) mutable {
            editorCamera.update();
            sceneManager.getActiveScene()->update();
            return !sceneManager.hasNewScene();
          },
          [&ui, &sceneManager, &renderData]() {
            ui.render(sceneManager);
            renderData->update();
          });
    }

    return 0;
  } catch (std::runtime_error error) {
    std::cerr << error.what() << std::endl;
    return 1;
  }
}
