#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/renderer/Renderer.h"
#include "liquid/renderer/Material.h"
#include "liquid/renderer/ShaderLibrary.h"

#include "liquid/window/Window.h"

#include "liquid/scene/Scene.h"
#include "liquid/scene/MeshInstance.h"
#include "liquid/scene/Light.h"

#include "liquid/profiler/FPSCounter.h"

#include "liquid/loaders/GLTFLoader.h"
#include "liquid/loaders/KtxTextureLoader.h"
#include "liquid/loaders/ImageTextureLoader.h"
#include "liquid/loop/MainLoop.h"

#include "platform-tools/NativeFileDialog.h"
#include "cube.h"

#include "ui/SceneHierarchy.h"
#include "ui/UILayer.h"
#include "EditorCamera.h"

#include "liquid/rhi/vulkan/VulkanRenderBackend.h"

#include <GLFW/glfw3.h>

static liquid::platform_tools::NativeFileDialog fileDialog;
std::list<liquid::String> sceneQueue;
bool changed = true;

liquid::Entity getNewSkybox(liquid::Window &window, const liquid::Mesh &mesh,
                            liquid::Renderer &renderer,
                            liquid::EntityContext &context) {
  liquid::KtxTextureLoader ktxLoader(renderer.getRegistry());
  liquid::ImageTextureLoader imageLoader(renderer.getRegistry());

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

  window.focus();

  auto entity = context.createEntity();

  const auto &material = renderer.createMaterial({environmentTexture}, {},
                                                 liquid::rhi::CullMode::Front);

  auto instance = std::make_shared<liquid::MeshInstance<liquid::Mesh>>(
      mesh, renderer.getRegistry());
  instance->setMaterial(material);

  context.setComponent<liquid::MeshComponent>(entity, {instance});
  context.setComponent<liquid::EnvironmentComponent>(
      entity, {irradianceTexture, specularTexture, lutTexture});

  return entity;
}

const auto moveSpeed = 0.5f;   // 0.5 m/s * 0.01s
const auto strafeSpeed = 0.5f; // m/s
const auto timeDelta = 1.0f;

int main() {
  liquid::EventSystem eventSystem;
  liquid::Engine::setAssetsPath(
      std::filesystem::path("./engine/assets").string());

  liquid::EntityContext context;
  liquid::Window window("Scene Viewer", 1024, 768, eventSystem);
  liquid::rhi::VulkanRenderBackend backend(window);

  liquid::Renderer renderer(context, window, backend.createDefaultDevice());

  EditorCamera editorCamera(context, renderer, window);

  eventSystem.observe(liquid::KeyboardEvent::Released,
                      [&editorCamera](const auto &data) {
                        int key = data.key;
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
                      });

  eventSystem.observe(liquid::KeyboardEvent::Pressed,
                      [&editorCamera](const auto &data) {
                        int key = data.key;

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

  liquid::GLTFLoader loader(context, renderer);

  liquid::FPSCounter fpsCounter;
  liquid::MainLoop mainLoop(window, fpsCounter);

  UILayer ui(context);

  while (!window.shouldClose()) {
    liquid::SharedPtr<liquid::Scene> scene = nullptr;
    scene = std::make_shared<liquid::Scene>(context);
    editorCamera.initEntity();

    if (!sceneQueue.empty()) {
      auto gltfFilename = sceneQueue.back();
      scene = std::make_shared<liquid::Scene>(context);
      auto &&result = loader.loadFromFile(gltfFilename);
      if (result.hasResult()) {
        scene->getRootNode()->addChild(result.getResult());
      } else {
        // TODO: Add logger
      }

      sceneQueue.pop_back();
    }

    ui.onSceneOpen([&window]() {
      liquid::String newFile = fileDialog.getFilePathFromDialog({"gltf"});
      window.focus();
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
    context.setComponent<liquid::NameComponent>(light1, {"Light 1"});
    liquid::TransformComponent ltransform{};
    ltransform.localRotation = glm::quat(glm::vec3(0, 0, glm::pi<float>()));
    auto *node = scene->getRootNode();
    node->addChild(light1, ltransform);

    liquid::SceneNode *environmentNode = nullptr;

    ui.onEnvironmentOpen([&window, &context, &cubeMesh, &renderer, &node,
                          &environmentNode]() mutable {
      auto environment = getNewSkybox(window, cubeMesh, renderer, context);

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

    const auto &renderData = renderer.prepareScene(scene.get());
    auto graph = renderer.createRenderGraph(renderData, true);

    {
      auto &pass = graph.first.addPass("fullscreenQuad");
      pass.write(graph.first.getSwapchain(), graph.second.defaultColor);
      pass.read(graph.second.mainColor);

      auto pipeline = renderer.getRegistry().setPipeline(
          {renderer.getShaderLibrary().getShader(
               "__engine.fullscreenQuad.default.vertex"),
           renderer.getShaderLibrary().getShader(
               "__engine.fullscreenQuad.default.fragment"),
           liquid::rhi::PipelineVertexInputLayout{},
           liquid::rhi::PipelineInputAssembly{},
           liquid::rhi::PipelineRasterizer{
               liquid::rhi::PolygonMode::Fill, liquid::rhi::CullMode::Front,
               liquid::rhi::FrontFace::CounterClockwise},
           liquid::rhi::PipelineColorBlend{
               {liquid::rhi::PipelineColorBlendAttachment{}}}});

      pass.addPipeline(pipeline);
      pass.setExecutor([pipeline, &graph](auto &commandList) {
        commandList.bindPipeline(pipeline);

        liquid::rhi::Descriptor descriptor;
        descriptor.bind(0, {graph.second.mainColor},
                        liquid::rhi::DescriptorType::CombinedImageSampler);
        commandList.bindDescriptor(pipeline, 0, descriptor);

        commandList.draw(3, 0);
      });
    }

    mainLoop.setRenderFn([&renderer, &ui, &graph]() {
      renderer.getImguiRenderer().beginRendering();
      ui.render();
      renderer.getImguiRenderer().endRendering();
      renderer.render(graph.first);
    });

    mainLoop.setUpdateFn([&ui, &scene, node, &editorCamera, &window,
                          &renderData, &eventSystem](double dt) mutable {
      eventSystem.poll();
      ImGuiIO &io = ImGui::GetIO();
      scene->update();
      renderData->update();

      editorCamera.update();

      node->getTransform().localRotation =
          glm::angleAxis(glm::radians(horizontalAngle),
                         glm::vec3(0.0f, 1.0f, 0.0f)) *
          glm::angleAxis(glm::radians(verticalAngle),
                         glm::vec3(1.0f, 0.0f, 0.0f));

      return !changed;
    });

    mainLoop.run();

    ui.getSceneHierarchy().setScene(nullptr);
    context.destroy();
  }

  renderer.wait();
  return 0;
}
