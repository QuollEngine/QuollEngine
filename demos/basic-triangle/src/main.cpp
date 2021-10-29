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

#include "loaders/ImageTextureLoader.h"

#include "loop/MainLoop.h"

uint32_t materialIndex = 0;
bool changed = false;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    materialIndex++;
    changed = true;
  }
}

int main() {
  try {
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
    auto &&shaderTextureVert =
        renderer->createShader("texture-shader.vert.spv");
    auto &&shaderTextureFrag =
        renderer->createShader("texture-shader.frag.spv");

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

    std::unique_ptr<liquid::Camera> camera(
        new liquid::Camera(renderer->getResourceAllocator()));
    std::unique_ptr<liquid::Scene> scene(new liquid::Scene(context));

    scene->setActiveCamera(camera.get());
    scene->getRootNode()->addChild(entity, glm::mat4{1.0f});

    glfwSetKeyCallback(window->getInstance(), key_callback);

    liquid::MainLoop mainLoop(renderer.get(), window.get());

    auto *instancePtr = instance.get();

    return mainLoop.run(
        scene.get(),
        [instancePtr, materials](double dt) mutable {
          if (changed) {
            instancePtr->setMaterial(
                materials[materialIndex % materials.size()]);
            changed = false;
          }

          return true;
        },
        []() {});
  } catch (std::runtime_error error) {
    std::cerr << error.what() << std::endl;
    return 1;
  }
}
