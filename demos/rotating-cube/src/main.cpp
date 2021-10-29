#include "core/Base.h"
#include "core/Engine.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "renderer/Material.h"

#include "renderer/vulkan/VulkanRenderer.h"

#include "window/glfw/GLFWWindow.h"

#include "scene/Vertex.h"
#include "scene/Mesh.h"
#include "scene/MeshInstance.h"
#include "scene/Camera.h"
#include "scene/Scene.h"

#include "loaders/ImageTextureLoader.h"

#include "loop/MainLoop.h"

#include "cube.h"

bool rotate = true;
bool materialChanged = false;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    rotate = !rotate;
  }

  if (key == GLFW_KEY_C && action == GLFW_PRESS) {
    materialChanged = true;
  }
}

int main() {
  try {
    liquid::Engine::setAssetsPath(
        std::filesystem::path("../../../../engine/bin/Debug/assets").string());
    liquid::EntityContext context;
    std::unique_ptr<liquid::GLFWWindow> window(
        new liquid::GLFWWindow("Rotating Cube", 640, 480));
    std::unique_ptr<liquid::VulkanRenderer> renderer(
        new liquid::VulkanRenderer(context, window.get(), true));

    liquid::ImageTextureLoader textureLoader(renderer->getResourceAllocator());

    auto &&materialTexture = renderer->createMaterial(
        renderer->createShader("texture-shader.vert.spv"),
        renderer->createShader("texture-shader.frag.spv"),
        {textureLoader.loadFromFile("brick.png")}, {}, liquid::CullMode::None);

    auto &&materialProperties = renderer->createMaterial(
        renderer->createShader("material-shader.vert.spv"),
        renderer->createShader("material-shader.frag.spv"), {},
        {{"diffuse", liquid::Property(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))},
         {"shininess", liquid::Property(0.2f)},
         {"specular", liquid::Property(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f))}},
        liquid::CullMode::None);

    liquid::Mesh mesh = createCube();

    std::array<liquid::SharedPtr<liquid::Material>, 2> materials{
        materialTexture, materialProperties};
    uint32_t materialIndex = 0;

    liquid::SharedPtr<liquid::MeshInstance> instance(
        new liquid::MeshInstance(&mesh, renderer->getResourceAllocator()));

    instance->setMaterial(materials[materialIndex]);
    auto entity = context.createEntity();
    context.setComponent<liquid::MeshComponent>(entity, {instance});

    glfwSetKeyCallback(window->getInstance(), key_callback);

    uint32_t frame = 0;

    std::unique_ptr<liquid::Camera> camera(
        new liquid::Camera(renderer->getResourceAllocator()));
    std::unique_ptr<liquid::Scene> scene(new liquid::Scene(context));

    scene->setActiveCamera(camera.get());

    auto *node = scene->getRootNode()->addChild(entity);

    const auto &fbSize = window->getFramebufferSize();

    camera->setPerspective(
        70.0f, static_cast<float>(fbSize.width) / fbSize.height, 0.1f, 200.0f);
    camera->lookAt({0.0, 0.0, -6.0}, {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0});

    window->addResizeHandler([&camera](uint32_t width, uint32_t height) {
      camera->setPerspective(70.0f, static_cast<float>(width) / height, 0.1f,
                             200.0f);
    });

    liquid::MainLoop mainLoop(renderer.get(), window.get());

    return mainLoop.run(
        scene.get(),
        [node, frame, materials, &instance, materialIndex,
         &renderer](double dt) mutable {
          if (materialChanged) {
            materialIndex++;
            materialChanged = false;
            instance->setMaterial(materials[materialIndex % materials.size()]);
          }
          node->setTransform(glm::rotate(glm::mat4{1.0f},
                                         glm::radians(frame * 0.4f),
                                         glm::vec3{0.0, 1.0, 1.0}));

          if (rotate) {
            frame++;
          }

          return true;
        },
        []() {});
  } catch (std::runtime_error error) {
    std::cerr << error.what() << std::endl;
    return 1;
  }
}
