#include "core/Base.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "renderer/Texture.h"
#include "renderer/Shader.h"
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

#include "createSphere.h"

bool rotate = true;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    rotate = !rotate;
  }
}

int main() {
  try {
    liquid::EntityContext context;

    std::unique_ptr<liquid::GLFWWindow> window(
        new liquid::GLFWWindow("Transforming Spheres", 640, 480));
    std::unique_ptr<liquid::VulkanRenderer> renderer(
        new liquid::VulkanRenderer(context, window.get(), true));

    liquid::Mesh &&mesh1 = createSphere(1.0, 10, 10, RED | GREEN | BLUE);
    liquid::Mesh &&mesh2 = createSphere(1.0, 10, 10, GREEN | BLUE);
    liquid::Mesh &&mesh3 = createSphere(1.0, 10, 10, RED | BLUE);
    liquid::Mesh &&mesh4 = createSphere(1.0, 10, 10, RED | GREEN);

    liquid::ImageTextureLoader textureLoader(renderer->getResourceAllocator());

    auto &&textureVertShader =
        renderer->createShader("texture-shader.vert.spv");
    auto &&textureFragShader =
        renderer->createShader("texture-shader.frag.spv");

    auto &&colorVertShader = renderer->createShader("basic-shader.vert.spv");
    auto &&colorFragShader = renderer->createShader("basic-shader.frag.spv");

    auto &&lavaMaterial = renderer->createMaterial(
        textureVertShader, textureFragShader,
        {textureLoader.loadFromFile("lava.jpg")}, {}, liquid::CullMode::None);
    auto &&waterMaterial = renderer->createMaterial(
        textureVertShader, textureFragShader,
        {textureLoader.loadFromFile("water.png")}, {}, liquid::CullMode::None);
    auto &&grassMaterial = renderer->createMaterial(
        textureVertShader, textureFragShader,
        {textureLoader.loadFromFile("grass.png")}, {}, liquid::CullMode::None);
    auto &&colorMaterial = renderer->createMaterial(
        colorVertShader, colorFragShader, {}, {}, liquid::CullMode::None);

    liquid::SharedPtr<liquid::MeshInstance> instance1(
        new liquid::MeshInstance(&mesh1, renderer->getResourceAllocator()));
    instance1->setMaterial(lavaMaterial);
    liquid::SharedPtr<liquid::MeshInstance> instance2(
        new liquid::MeshInstance(&mesh2, renderer->getResourceAllocator()));
    instance2->setMaterial(waterMaterial);
    liquid::SharedPtr<liquid::MeshInstance> instance3(
        new liquid::MeshInstance(&mesh3, renderer->getResourceAllocator()));
    instance3->setMaterial(grassMaterial);
    liquid::SharedPtr<liquid::MeshInstance> instance4(
        new liquid::MeshInstance(&mesh4, renderer->getResourceAllocator()));
    instance4->setMaterial(colorMaterial);
    liquid::SharedPtr<liquid::MeshInstance> instance5(
        new liquid::MeshInstance(&mesh2, renderer->getResourceAllocator()));

    auto e1 = context.createEntity();
    auto e2 = context.createEntity();
    auto e3 = context.createEntity();
    auto e4 = context.createEntity();
    auto e5 = context.createEntity();

    context.setComponent<liquid::MeshComponent>(e1, {instance1});
    context.setComponent<liquid::MeshComponent>(e2, {instance2});
    context.setComponent<liquid::MeshComponent>(e3, {instance3});
    context.setComponent<liquid::MeshComponent>(e4, {instance4});
    context.setComponent<liquid::MeshComponent>(e5, {instance5});

    glfwSetKeyCallback(window->getInstance(), key_callback);

    std::unique_ptr<liquid::Camera> camera(
        new liquid::Camera(renderer->getResourceAllocator()));
    std::unique_ptr<liquid::Scene> scene(new liquid::Scene(context));

    scene->setActiveCamera(camera.get());

    auto *node = scene->getRootNode()->addChild(e1);
    auto *child1 = node->addChild(e2);
    auto *child1_1 = child1->addChild(e5);
    auto *child2 = node->addChild(e3);
    auto *child3 = node->addChild(e4);

    const auto &fbSize = window->getFramebufferSize();

    camera->setPerspective(
        70.0, static_cast<float>(fbSize.width) / fbSize.height, 0.1, 200.0);
    camera->lookAt({0.0, 0.0, -5.0}, {0.0, 0.0, 1.0}, {0.0, 1.0, 0.0});

    window->addResizeHandler([&camera](uint32_t width, uint32_t height) {
      camera->setPerspective(70.0, static_cast<float>(width) / height, 0.1,
                             200.0);
    });

    uint32_t frame = 0;

    liquid::MainLoop mainLoop(renderer.get(), window.get());

    mainLoop.run(
        scene.get(), [node, child1, child2, child3, child1_1, frame]() mutable {
          node->setTransform(glm::rotate(glm::mat4{1.0f},
                                         glm::radians(frame * 0.4f),
                                         glm::vec3{1.0, 1.0, 1.0}));

          child1->setTransform(
              glm::translate(glm::mat4{1.0}, glm::vec3{2.0, 0.0, 0.0}) *
              glm::rotate(glm::mat4{1.0f}, glm::radians(frame * 0.9f),
                          glm::vec3{0.0, 0.0, -1.0}) *
              glm::scale(glm::mat4{1.0}, glm::vec3{0.4, 0.4, 0.4}));

          child1_1->setTransform(
              glm::translate(glm::mat4{1.0}, glm::vec3{0.0, 2.0, 0.0}) *
              glm::rotate(glm::mat4{1.0f}, glm::radians(frame * 0.9f),
                          glm::vec3{0.0, 0.0, -1.0}) *
              glm::scale(glm::mat4{1.0}, glm::vec3{0.4, 0.4, 0.4}));

          child2->setTransform(
              glm::translate(glm::mat4{1.0}, glm::vec3{-2.0, 0.0, 0.0}) *
              glm::rotate(glm::mat4{1.0f}, glm::radians(frame * 0.9f),
                          glm::vec3{0.0, 0.0, -1.0}) *
              glm::scale(glm::mat4{1.0}, glm::vec3{0.4, 0.4, 0.4}));

          child3->setTransform(
              glm::translate(glm::mat4{1.0}, glm::vec3{0.0, 2.0, 0.0}) *
              glm::rotate(glm::mat4{1.0f}, glm::radians(frame * 0.9f),
                          glm::vec3{0.0, 0.0, -1.0}) *
              glm::scale(glm::mat4{1.0}, glm::vec3{0.4, 0.4, 0.4}));

          if (rotate) {
            frame++;
          }

          return true;
        });
    return 0;
  } catch (std::runtime_error error) {
    std::cerr << error.what() << std::endl;
    return 1;
  }
}
