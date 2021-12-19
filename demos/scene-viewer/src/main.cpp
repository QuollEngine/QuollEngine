#include "core/Base.h"
#include "core/Engine.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "renderer/vulkan/VulkanRenderer.h"
#include "renderer/Material.h"
#include "renderer/ShaderLibrary.h"

#include "window/glfw/GLFWWindow.h"

#include "scene/Scene.h"
#include "scene/MeshInstance.h"
#include "scene/Light.h"

#include "loaders/TinyGLTFLoader.h"
#include "loaders/KtxTextureLoader.h"
#include "loaders/ImageTextureLoader.h"
#include "loop/MainLoop.h"

#include "platform-tools/NativeFileDialog.h"
#include "cube.h"

#include "ui/SceneHierarchy.h"
#include "ui/UILayer.h"
#include "EditorCamera.h"

#include <stb/stb_image.h>

static liquid::platform_tools::NativeFileDialog fileDialog;
std::list<liquid::String> sceneQueue;
bool changed = true;

bool leftMouseBtnPressed = false;

liquid::Entity getNewSkybox(GLFWwindow *window, liquid::Mesh *mesh,
                            liquid::VulkanRenderer *renderer,
                            liquid::EntityContext &context) {
  liquid::KtxTextureLoader ktxLoader(renderer->getResourceAllocator());
  liquid::ImageTextureLoader imageLoader(renderer->getResourceAllocator());

  liquid::String envPath = fileDialog.getFilePathFromDialog({"ktx", "ktx2"});

  if (envPath.empty()) {
    return std::numeric_limits<liquid::Entity>::max();
  }
  auto environmentTexture = ktxLoader.loadFromFile(envPath);

  liquid::String irradiancePath =
      fileDialog.getFilePathFromDialog({"ktx", "ktx2"});
  if (irradiancePath.empty()) {
    return std::numeric_limits<liquid::Entity>::max();
  }
  auto irradianceTexture = ktxLoader.loadFromFile(irradiancePath);

  liquid::String specularPath =
      fileDialog.getFilePathFromDialog({"ktx", "ktx2"});
  if (specularPath.empty()) {
    return std::numeric_limits<liquid::Entity>::max();
  }
  auto specularTexture = ktxLoader.loadFromFile(specularPath);

  liquid::String lutPath = fileDialog.getFilePathFromDialog({"png"});
  if (lutPath.empty()) {
    return std::numeric_limits<liquid::Entity>::max();
  }
  auto lutTexture = imageLoader.loadFromFile(lutPath);

  glfwFocusWindow(window);

  auto entity = context.createEntity();

  const auto &material = renderer->createMaterial(
      renderer->getShaderLibrary()->getShader("__engine.default.skybox.vertex"),
      renderer->getShaderLibrary()->getShader(
          "__engine.default.skybox.fragment"),
      {environmentTexture}, {}, liquid::CullMode::Front);

  auto instance = std::make_shared<liquid::MeshInstance>(
      mesh, renderer->getResourceAllocator());
  instance->setMaterial(material);

  context.setComponent<liquid::MeshComponent>(entity, {instance});
  context.setComponent<liquid::EnvironmentComponent>(
      entity, {irradianceTexture, specularTexture, lutTexture});

  return entity;
}

void mouse_callback(GLFWwindow *window, int button, int action, int mods) {
  if (action == GLFW_RELEASE) {
    leftMouseBtnPressed = false;
  }

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    leftMouseBtnPressed = true;
  }
}

const auto moveSpeed = 0.5f;   // 0.5 m/s * 0.01s
const auto strafeSpeed = 0.5f; // m/s
const auto timeDelta = 1.0f;

int main() {
  liquid::Engine::setAssetsPath(
      std::filesystem::path("../../../../engine/bin/Debug/assets").string());

  liquid::EntityContext context;
  std::unique_ptr<liquid::GLFWWindow> window(
      new liquid::GLFWWindow("Scene Viewer", 1024, 768));
  std::unique_ptr<liquid::VulkanRenderer> renderer(
      new liquid::VulkanRenderer(context, window.get(), true));

  glfwSetMouseButtonCallback(window->getInstance(), mouse_callback);

  EditorCamera editorCamera(context, renderer.get(), window.get());

  window->addKeyHandler(
      [&editorCamera](int key, int scancode, int action, int mods) {
        if (action == GLFW_RELEASE) {
          if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
            editorCamera.move(0.0f);
          }

          if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
            editorCamera.strafe(0.0f);
          }

          if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
            editorCamera.yaw(0.0f);
          }
          if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
            editorCamera.pitch(0.0f);
          }

          return;
        }

        if (key == GLFW_KEY_W) {
          editorCamera.move(moveSpeed * timeDelta);
        } else if (key == GLFW_KEY_S) {
          editorCamera.move(-moveSpeed * timeDelta);
        }

        if (key == GLFW_KEY_A) {
          editorCamera.strafe(-strafeSpeed * timeDelta);
        } else if (key == GLFW_KEY_D) {
          editorCamera.strafe(strafeSpeed * timeDelta);
        }

        if (key == GLFW_KEY_LEFT) {
          editorCamera.yaw(-2.0f * timeDelta);
        } else if (key == GLFW_KEY_RIGHT) {
          editorCamera.yaw(2.0f * timeDelta);
        }

        if (key == GLFW_KEY_DOWN) {
          editorCamera.pitch(-2.0f * timeDelta);
        } else if (key == GLFW_KEY_UP) {
          editorCamera.pitch(2.0f * timeDelta);
        }
      });

  auto cubeMesh = createCube();

  liquid::TinyGLTFLoader loader(context, renderer.get());

  liquid::MainLoop mainLoop(renderer.get(), window.get());

  UILayer ui(context);

  while (!window->shouldClose()) {
    liquid::SharedPtr<liquid::Scene> scene = nullptr;
    scene = std::make_shared<liquid::Scene>(context);
    editorCamera.initEntity();

    if (!sceneQueue.empty()) {
      auto gltfFilename = sceneQueue.back();
      scene = std::make_shared<liquid::Scene>(context);
      scene->getRootNode()->addChild(loader.loadFromFile(gltfFilename));

      sceneQueue.pop_back();
    }

    ui.onSceneOpen([&window]() {
      liquid::String newFile = fileDialog.getFilePathFromDialog({"gltf"});
      glfwFocusWindow(window->getInstance());
      if (!newFile.empty()) {
        sceneQueue.push_back(newFile);
        changed = true;
      }
    });

    scene->setActiveCamera(editorCamera.getCamera());

    ui.getSceneHierarchy().setScene(scene);

    changed = false;

    auto light1 = context.createEntity();

    context.setComponent<liquid::LightComponent>(
        light1, {std::make_shared<liquid::Light>(
                    liquid::Light::DIRECTIONAL, glm::vec3{0.0f, 0.5f, 0.5f},
                    glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, 1.0f)});

    auto *node = scene->getRootNode();
    node->addChild(light1);

    liquid::SceneNode *environmentNode = nullptr;

    ui.onEnvironmentOpen([&window, &context, &cubeMesh, &renderer, &node,
                          &environmentNode]() mutable {
      auto environment = getNewSkybox(window->getInstance(), &cubeMesh,
                                      renderer.get(), context);

      if (environment == std::numeric_limits<liquid::Entity>::max()) {
        return;
      }

      context.setComponent<liquid::NameComponent>(environment, {"Environment"});

      if (environmentNode) {
        environmentNode->setEntity(environment);
      } else {
        environmentNode = node->addChild(environment);
      }
    });

    static float horizontalAngle = 0, verticalAngle = 0;
    static double prevX = 0.0, prevY = 0.0;

    const auto &renderData = renderer->prepareScene(scene.get());
    liquid::RenderGraph graph = renderer->createRenderGraph(renderData);

    mainLoop.run(
        graph,
        [&ui, &scene, node, &editorCamera, &window](double dt) mutable {
          ImGuiIO &io = ImGui::GetIO();
          scene->update();

          if (leftMouseBtnPressed && !io.WantCaptureMouse) {
            double xpos, ypos;
            glfwGetCursorPos(window->getInstance(), &xpos, &ypos);

            const auto &size = window->getWindowSize();
            float width = (float)size.width;
            float height = (float)size.height;

            if (xpos < width && xpos >= 0 && ypos < height && ypos >= 0) {
              float x = (xpos / width) * 5;
              float y = (ypos / height) * 5;

              if (abs(xpos - prevX) > 1.0) {
                if (xpos < prevX) {
                  horizontalAngle -= x;
                } else {
                  horizontalAngle += x;
                }
              }

              if (abs(ypos - prevY) > 1.0) {
                if (ypos < prevY) {
                  verticalAngle -= y;
                } else {
                  verticalAngle += y;
                }
              }

              prevX = xpos;
              prevY = ypos;
            }
          }

          editorCamera.update();

          node->setTransform(
              glm::rotate(glm::mat4{1.0f}, glm::radians(horizontalAngle),
                          glm::vec3{0.0, 1.0, 0.0}) *
              glm::rotate(glm::mat4{1.0f}, glm::radians(verticalAngle),
                          glm::vec3{1.0, 0.0, 0.0}));

          return !changed;
        },
        [&ui]() { ui.render(); });

    ui.getSceneHierarchy().setScene(nullptr);
    context.destroy();
  }
  return 0;
}
