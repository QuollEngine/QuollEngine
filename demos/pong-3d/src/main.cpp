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

#include "liquid/rhi/vulkan/VulkanRenderBackend.h"

class Game {
public:
  Game()
      : window("Pong 3D", 800, 600, eventSystem), backend(window),
        renderer(assetManager.getRegistry(), window,
                 backend.createDefaultDevice()),
        presenter(renderer.getShaderLibrary(), renderer.getRegistry()),
        physicsSystem(eventSystem),
        assetManager(std::filesystem::current_path()),
        scriptingSystem(eventSystem, assetManager.getRegistry()) {

    assetManager.preloadAssets(renderer.getRegistry());

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

    liquid::SceneRenderer sceneRenderer(renderer.getShaderLibrary(),
                                        renderer.getRegistry(),
                                        assetManager.getRegistry());

    const auto &passData = sceneRenderer.attach(graph);

    mainLoop.setUpdateFn([=](float dt) mutable {
      scriptingSystem.start(entityContext);
      eventSystem.poll();
      scriptingSystem.update(dt, entityContext);

      auto &component =
          entityContext.getComponent<liquid::CameraComponent>(cameraEntity);

      if (gameEnded) {
        auto &transform =
            entityContext.getComponent<liquid::LocalTransformComponent>(ball);
        transform.localPosition = glm::vec3{0.0f, 0.0f, 0.0f};
        gameEnded = false;
      }
      updateScene();
      physicsSystem.update(dt, entityContext);
      updateGameLogic(dt);

      return true;
    });

    mainLoop.setRenderFn([this, &graph, &passData, &sceneRenderer]() {
      const auto &renderFrame = renderer.getRenderDevice()->beginFrame();

      if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
        sceneRenderer.updateFrameData(entityContext, cameraEntity);

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
  void updateScene() {
    {
      auto &transform =
          entityContext.getComponent<liquid::LocalTransformComponent>(p2);
      transform.localPosition.x = botPosition;
    }

    sceneUpdater.update(entityContext);
  }

  void updateGameLogic(float dt) {
    static bool firstTime = true;
    if (firstTime) {
      auto &rigidBody =
          entityContext.getComponent<liquid::RigidBodyComponent>(ball);
      rigidBody.actor->addForce({0.0f, 0.0f, 800.0f});
      rigidBody.actor->addTorque({10.0f, 0.0f, 0.0f});

      firstTime = !firstTime;
    }

    auto &ballTransform =
        entityContext.getComponent<liquid::LocalTransformComponent>(ball);

    if (abs(botPosition - ballTransform.localPosition.x) < 0.2f) {
      botVelocity = 0.0;
    } else if (botPosition > ballTransform.localPosition.x) {
      botVelocity = -velocity;
    } else {
      botVelocity = velocity;
    }

    float newBotPosition = botPosition + botVelocity * dt;
    if (newBotPosition - paddleWidth > wallLineRight &&
        newBotPosition + paddleWidth < wallLineLeft) {
      botPosition = newBotPosition;
    }
  }

  void setupScene() {
    cameraEntity = entityContext.createEntity();
    entityContext.setComponent<liquid::CameraComponent>(cameraEntity, {});

    const auto &fbSize = window.getFramebufferSize();

    auto &camera =
        entityContext.getComponent<liquid::CameraComponent>(cameraEntity);

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

    auto e1 = entityContext.createEntity();
    auto e2 = entityContext.createEntity();
    auto e3 = entityContext.createEntity();
    auto e4 = entityContext.createEntity();

    ball = entityContext.createEntity();
    p1 = entityContext.createEntity();
    p2 = entityContext.createEntity();

    entityContext.setComponent<liquid::MeshComponent>(e1, {barMesh});
    entityContext.setComponent<liquid::MeshComponent>(e2, {barMesh});
    entityContext.setComponent<liquid::MeshComponent>(e3, {barMesh});
    entityContext.setComponent<liquid::MeshComponent>(e4, {barMesh});

    entityContext.setComponent<liquid::CollidableComponent>(
        e1, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                liquid::PhysicsGeometryType::Box,
                liquid::PhysicsGeometryBox{glm::vec3(5.0f, 0.2f, 0.1f)}}});

    entityContext.setComponent<liquid::CollidableComponent>(
        e2, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                liquid::PhysicsGeometryType::Box,
                liquid::PhysicsGeometryBox{glm::vec3(3.6f, 0.2f, 0.1f)}}});

    entityContext.setComponent<liquid::CollidableComponent>(
        e3, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                liquid::PhysicsGeometryType::Box,
                liquid::PhysicsGeometryBox{glm::vec3(3.6f, 0.2f, 0.1f)}}});

    entityContext.setComponent<liquid::CollidableComponent>(
        e4, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                liquid::PhysicsGeometryType::Box,
                liquid::PhysicsGeometryBox{glm::vec3(5.0f, 0.2f, 0.1f)}}});

    entityContext.setComponent<liquid::CollidableComponent>(
        ball, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                  liquid::PhysicsGeometryType::Sphere,
                  liquid::PhysicsGeometrySphere{ballRadius}}});

    entityContext.setComponent<liquid::RigidBodyComponent>(
        ball,
        liquid::RigidBodyComponent{{1.0f, {0.05f, 100.0f, 100.0f}, false}});

    entityContext.setComponent<liquid::MeshComponent>(p1, {barMesh});
    entityContext.setComponent<liquid::MeshComponent>(p2, {barMesh});
    entityContext.setComponent<liquid::MeshComponent>(ball, {ballMesh});

    entityContext.setComponent(
        e1, createWallTransform({0.0f, 0.0f, 3.5f}, 0.0f, 5.0f));
    entityContext.setComponent(
        e2, createWallTransform({-5.0f, 0.0f, 0.0f}, 90.0f, 3.6f));
    entityContext.setComponent(
        e3, createWallTransform({5.0f, 0.0f, 0.0f}, 90.0f, 3.6f));
    entityContext.setComponent(
        e4, createWallTransform({0.0f, 0.0f, -3.5f}, 0.0f, 5.0f));

    entityContext.setComponent<liquid::WorldTransformComponent>(e1, {});
    entityContext.setComponent<liquid::WorldTransformComponent>(e2, {});
    entityContext.setComponent<liquid::WorldTransformComponent>(e3, {});
    entityContext.setComponent<liquid::WorldTransformComponent>(e4, {});

    eventSystem.observe(
        liquid::CollisionEvent::CollisionStarted,
        [e1, e4, this](const liquid::CollisionObject &data) {
          if ((data.a == ball || data.b == ball) &&
              (data.a == e1 || data.b == e1 || data.a == e4 || data.b == e4)) {
            gameEnded = true;
          }
        });

    // Create paddles
    {
      auto [_, playerScriptHandle] = assetManager.getRegistry().getAssetByPath(
          assetManager.getAssetsPath() / "player.lua");

      entityContext.setComponent<liquid::ScriptingComponent>(
          p1, {static_cast<liquid::LuaScriptAssetHandle>(playerScriptHandle)});

      entityContext.setComponent<liquid::CollidableComponent>(
          p1, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                  liquid::PhysicsGeometryType::Box,
                  liquid::PhysicsGeometryBox{glm::vec3(1.0f, 0.2f, 0.1f)}}});

      entityContext.setComponent<liquid::LocalTransformComponent>(p1, {});
      entityContext.setComponent<liquid::WorldTransformComponent>(p1, {});
    }

    {
      liquid::LocalTransformComponent transform;
      transform.localPosition = glm::vec3(0.0, 0.0f, 3.0f);
      transform.localScale = glm::vec3{1.0f, 0.2f, 0.1f};

      entityContext.setComponent<liquid::CollidableComponent>(
          p2, liquid::CollidableComponent{
                  liquid::PhysicsGeometryDesc{
                      liquid::PhysicsGeometryType::Box,
                      liquid::PhysicsGeometryBox{glm::vec3(1.0f, 0.2f, 0.1f)}},
                  liquid::PhysicsMaterialDesc{0.0f, 0.0f, 1.0f}});

      entityContext.setComponent(p2, transform);
      entityContext.setComponent<liquid::WorldTransformComponent>(p2, {});
    }

    // create ball
    liquid::LocalTransformComponent ballTransform{};
    ballTransform.localScale = glm::vec3(0.3f);
    entityContext.setComponent(ball, ballTransform);
    entityContext.setComponent<liquid::WorldTransformComponent>(ball, {});
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
  liquid::EntityContext entityContext;
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
  bool gameEnded = false;
  // Game specific parameters
  float velocity = 5.0f;

  float ballRadius = 0.3f;

  // wall center position - width
  float wallLineLeft = 5.0f - 0.1f;
  float wallLineRight = -5.0f + 0.1f;

  // cube width / 2
  float paddleWidth = 1.0f;

  float safeAreaTop = 2.7f;
  float safeAreaBottom = -2.7f;

  float botVelocity = 0.0f;
  float botPosition = 0.0f;
  glm::vec3 ballPosition{0.0f, 0.0f, 0.0f};
  glm::vec3 ballVelocity{0.2f, 0.0f, 0.3f};
};

int main() {
  liquid::Engine::setAssetsPath(
      std::filesystem::path("./engine/assets").string());
  Game game;
  return game.run();
}
