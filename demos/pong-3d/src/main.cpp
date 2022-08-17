#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/renderer/Renderer.h"
#include "liquid/renderer/Presenter.h"
#include "liquid/renderer/SceneRenderer.h"
#include "liquid/window/Window.h"
#include "liquid/profiler/FPSCounter.h"
#include "liquid/scene/AutoAspectRatioComponent.h"
#include "liquid/physics/PhysicsSystem.h"

#include "liquid/scene/SceneUpdater.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/scripting/ScriptingSystem.h"

#include "liquid/loop/MainLoop.h"

#include "liquid/rhi-vulkan/VulkanRenderBackend.h"

class Game {
public:
  Game()
      : window("Pong 3D", 800, 600, eventSystem), backend(window),
        renderer(assetManager.getRegistry(), window,
                 backend.createDefaultDevice()),
        presenter(renderer.getShaderLibrary(), renderer.getRegistry()),
        physicsSystem(eventSystem),
        assetManager(std::filesystem::current_path(), true),
        scriptingSystem(eventSystem, assetManager.getRegistry()) {

    assetManager.preloadAssets(renderer.getRegistry(),
                               renderer.getRenderDevice());

    for (auto &[handle, mesh] :
         assetManager.getRegistry().getMeshes().getAssets()) {
      if (mesh.name == "cube.lqmesh") {
        barMesh = handle;
      } else if (mesh.name == "sphere.lqmesh") {
        ballMesh = handle;
      }
    }

    setupScene();
  }

  int run() {
    liquid::FPSCounter fpsCounter;
    liquid::MainLoop mainLoop(window, fpsCounter);

    presenter.updateFramebuffers(renderer.getRenderDevice()->getSwapchain());

    liquid::rhi::RenderGraph graph;

    graph.setFramebufferExtent(window.getFramebufferSize());

    window.addResizeHandler([&graph](auto width, auto height) {
      graph.setFramebufferExtent({width, height});
    });

    liquid::SceneRenderer sceneRenderer(
        renderer.getShaderLibrary(), renderer.getRegistry(),
        assetManager.getRegistry(), renderer.getRenderDevice());

    sceneRenderer.setClearColor(glm::vec4(0.46f, 0.60f, 0.70f, 1.0f));

    const auto &passData = sceneRenderer.attach(graph);
    sceneRenderer.attachText(graph, passData);

    mainLoop.setUpdateFn([=](float dt) mutable {
      scriptingSystem.start(entityDatabase);
      eventSystem.poll();
      scriptingSystem.update(dt, entityDatabase);

      auto &transform =
          entityDatabase.getComponent<liquid::LocalTransformComponent>(ball);

      sceneUpdater.update(entityDatabase);
      physicsSystem.update(dt, entityDatabase);

      return true;
    });

    mainLoop.setRenderFn([this, &graph, &passData, &sceneRenderer]() {
      const auto &renderFrame = renderer.getRenderDevice()->beginFrame();

      if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
        sceneRenderer.updateFrameData(entityDatabase, cameraEntity);

        renderer.render(graph, renderFrame.commandList);

        presenter.present(renderFrame.commandList, passData.sceneColor,
                          renderFrame.swapchainImageIndex);

        renderer.getRenderDevice()->endFrame(renderFrame);
      } else {
        presenter.updateFramebuffers(
            renderer.getRenderDevice()->getSwapchain());
      }
    });

    mainLoop.run();
    renderer.wait();
    return 0;
  }

private:
  void setupScene() {
    cameraEntity = entityDatabase.createEntity();
    entityDatabase.setComponent<liquid::CameraComponent>(cameraEntity, {});

    const auto &fbSize = window.getFramebufferSize();

    auto &camera =
        entityDatabase.getComponent<liquid::CameraComponent>(cameraEntity);

    camera.projectionMatrix = glm::perspective(
        70.0f, static_cast<float>(fbSize.x) / static_cast<float>(fbSize.y),
        0.1f, 200.0f);
    camera.projectionMatrix[1][1] *= -1.0f;
    camera.viewMatrix = glm::lookAt(glm::vec3{0.0f, 4.0f, -8.0f},
                                    {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    camera.projectionViewMatrix = camera.projectionMatrix * camera.viewMatrix;

    window.addResizeHandler([this, &camera](uint32_t width,
                                            uint32_t height) mutable {
      camera.projectionMatrix = glm::perspective(
          70.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f,
          200.0f);
      camera.projectionMatrix[1][1] *= -1.0f;
      camera.projectionViewMatrix = camera.projectionMatrix * camera.viewMatrix;
    });

    auto e1 = entityDatabase.createEntity();
    auto e2 = entityDatabase.createEntity();
    auto e3 = entityDatabase.createEntity();
    auto e4 = entityDatabase.createEntity();

    ball = entityDatabase.createEntity();
    p1 = entityDatabase.createEntity();
    p2 = entityDatabase.createEntity();

    // Walls
    {
      entityDatabase.setComponent<liquid::MeshComponent>(e1, {barMesh});
      entityDatabase.setComponent<liquid::MeshComponent>(e2, {barMesh});
      entityDatabase.setComponent<liquid::MeshComponent>(e3, {barMesh});
      entityDatabase.setComponent<liquid::MeshComponent>(e4, {barMesh});

      entityDatabase.setComponent<liquid::CollidableComponent>(
          e1, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                  liquid::PhysicsGeometryType::Box,
                  liquid::PhysicsGeometryBox{glm::vec3(5.0f, 0.2f, 0.1f)}}});
      entityDatabase.setComponent<liquid::NameComponent>(e1, {"Bot wall"});

      entityDatabase.setComponent<liquid::CollidableComponent>(
          e2, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                  liquid::PhysicsGeometryType::Box,
                  liquid::PhysicsGeometryBox{glm::vec3(3.6f, 0.2f, 0.1f)}}});

      entityDatabase.setComponent<liquid::CollidableComponent>(
          e3, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                  liquid::PhysicsGeometryType::Box,
                  liquid::PhysicsGeometryBox{glm::vec3(3.6f, 0.2f, 0.1f)}}});

      entityDatabase.setComponent<liquid::CollidableComponent>(
          e4, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                  liquid::PhysicsGeometryType::Box,
                  liquid::PhysicsGeometryBox{glm::vec3(5.0f, 0.2f, 0.1f)}}});
      entityDatabase.setComponent<liquid::NameComponent>(e4, {"Player wall"});

      entityDatabase.setComponent(
          e1, createWallTransform({0.0f, 0.0f, 3.5f}, 0.0f, 5.0f));
      entityDatabase.setComponent(
          e2, createWallTransform({-5.0f, 0.0f, 0.0f}, 90.0f, 3.6f));
      entityDatabase.setComponent(
          e3, createWallTransform({5.0f, 0.0f, 0.0f}, 90.0f, 3.6f));
      entityDatabase.setComponent(
          e4, createWallTransform({0.0f, 0.0f, -3.5f}, 0.0f, 5.0f));

      entityDatabase.setComponent<liquid::WorldTransformComponent>(e1, {});
      entityDatabase.setComponent<liquid::WorldTransformComponent>(e2, {});
      entityDatabase.setComponent<liquid::WorldTransformComponent>(e3, {});
      entityDatabase.setComponent<liquid::WorldTransformComponent>(e4, {});
    }

    entityDatabase.setComponent<liquid::CollidableComponent>(
        ball, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                  liquid::PhysicsGeometryType::Sphere,
                  liquid::PhysicsGeometrySphere{ballRadius}}});

    {
      liquid::LocalTransformComponent ballTransform{};
      ballTransform.localScale = glm::vec3(0.3f);
      entityDatabase.setComponent(ball, ballTransform);
      entityDatabase.setComponent<liquid::NameComponent>(ball, {"Ball"});
      entityDatabase.setComponent<liquid::WorldTransformComponent>(ball, {});

      auto [_, ballScriptHandle] = assetManager.getRegistry().getAssetByPath(
          assetManager.getAssetsPath() / "ball.lua");
      entityDatabase.setComponent<liquid::RigidBodyComponent>(
          ball,
          liquid::RigidBodyComponent{{1.0f, {0.05f, 100.0f, 100.0f}, false}});
      entityDatabase.setComponent<liquid::ScriptingComponent>(
          ball, {static_cast<liquid::LuaScriptAssetHandle>(ballScriptHandle)});
    }

    entityDatabase.setComponent<liquid::MeshComponent>(p1, {barMesh});
    entityDatabase.setComponent<liquid::MeshComponent>(p2, {barMesh});
    entityDatabase.setComponent<liquid::MeshComponent>(ball, {ballMesh});

    // Create paddles
    {
      auto [_, playerScriptHandle] = assetManager.getRegistry().getAssetByPath(
          assetManager.getAssetsPath() / "player.lua");

      entityDatabase.setComponent<liquid::ScriptingComponent>(
          p1, {static_cast<liquid::LuaScriptAssetHandle>(playerScriptHandle)});

      entityDatabase.setComponent<liquid::CollidableComponent>(
          p1, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                  liquid::PhysicsGeometryType::Box,
                  liquid::PhysicsGeometryBox{glm::vec3(1.0f, 0.2f, 0.1f)}}});

      entityDatabase.setComponent<liquid::LocalTransformComponent>(p1, {});
      entityDatabase.setComponent<liquid::WorldTransformComponent>(p1, {});
    }

    {
      auto [_, botScriptHandle] = assetManager.getRegistry().getAssetByPath(
          assetManager.getAssetsPath() / "bot.lua");

      entityDatabase.setComponent<liquid::ScriptingComponent>(
          p2, {static_cast<liquid::LuaScriptAssetHandle>(botScriptHandle)});

      entityDatabase.setComponent<liquid::CollidableComponent>(
          p2, liquid::CollidableComponent{
                  liquid::PhysicsGeometryDesc{
                      liquid::PhysicsGeometryType::Box,
                      liquid::PhysicsGeometryBox{glm::vec3(1.0f, 0.2f, 0.1f)}},
                  liquid::PhysicsMaterialDesc{0.0f, 0.0f, 1.0f}});

      entityDatabase.setComponent<liquid::LocalTransformComponent>(p2, {});
      entityDatabase.setComponent<liquid::WorldTransformComponent>(p2, {});
    }
  }

  liquid::LocalTransformComponent
  createWallTransform(glm::vec3 position, float rotation, float scaleX) {
    liquid::LocalTransformComponent transform{};
    transform.localPosition = position;
    transform.localRotation =
        glm::angleAxis(glm::radians(rotation), glm::vec3(0, 1, 0));

    transform.localScale = glm::vec3(scaleX, 0.2f, 0.1f);
    return transform;
  }

private:
  liquid::EventSystem eventSystem;
  liquid::EntityDatabase entityDatabase;
  liquid::AssetManager assetManager;
  liquid::Window window;
  liquid::rhi::VulkanRenderBackend backend;
  liquid::Renderer renderer;
  liquid::Presenter presenter;
  liquid::PhysicsSystem physicsSystem;
  liquid::ScriptingSystem scriptingSystem;

  liquid::Entity cameraEntity = liquid::EntityNull;
  liquid::SceneUpdater sceneUpdater;

  liquid::MeshAssetHandle barMesh, ballMesh;

  liquid::Entity p1, p2, ball;

private:
  float ballRadius = 0.3f;

  // wall center position - width
  float wallLineLeft = 5.0f - 0.1f;
  float wallLineRight = -5.0f + 0.1f;

  // cube width / 2
  float paddleWidth = 1.0f;

  float safeAreaTop = 2.7f;
  float safeAreaBottom = -2.7f;

  glm::vec3 ballPosition{0.0f, 0.0f, 0.0f};
  glm::vec3 ballVelocity{0.2f, 0.0f, 0.3f};
};

int main() {
  liquid::Engine::setPath(std::filesystem::current_path() / "engine");
  Game game;
  return game.run();
}
