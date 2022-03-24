#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "liquid/renderer/Renderer.h"
#include "liquid/renderer/SceneRenderer.h"
#include "liquid/window/Window.h"

#include "liquid/scene/Vertex.h"
#include "liquid/scene/Mesh.h"
#include "liquid/scene/MeshInstance.h"
#include "liquid/scene/Camera.h"
#include "liquid/scene/Scene.h"

#include "liquid/loop/MainLoop.h"

#include "cube.h"
#include "sphere.h"

#include "liquid/rhi/vulkan/VulkanRenderBackend.h"

#include "liquid/physics/PhysicsSystem.h"

#include <GLFW/glfw3.h>

class Game {
public:
  Game()
      : window("Pong 3D", 800, 600), backend(window),
        renderer(entityContext, window, backend.createDefaultDevice()),
        physicsSystem(entityContext, eventSystem),
        vertexShader(
            renderer.getRegistry().setShader({"basic-shader.vert.spv"})),
        fragmentShader(
            renderer.getRegistry().setShader({"basic-shader.frag.spv"})),
        material(renderer.createMaterial({}, {}, liquid::rhi::CullMode::None)),
        camera(new liquid::Camera(&renderer.getRegistry())) {

    scene.reset(new liquid::Scene(entityContext));

    liquid::Mesh barMesh = createCube();
    liquid::Mesh ballMesh = createSphere(ballRadius, 10, 10, RED);

    window.addKeyHandler([this](int key, int scancode, int action, int mods) {
      handleKeyClick(key, scancode, action, mods);
    });

    barInstance.reset(new liquid::MeshInstance<liquid::Mesh>(
        barMesh, renderer.getRegistry()));
    barInstance->setMaterial(material);

    ballInstance.reset(new liquid::MeshInstance<liquid::Mesh>(
        ballMesh, renderer.getRegistry()));
    ballInstance->setMaterial(material);

    setupScene();
  }

  int run() {
    liquid::MainLoop mainLoop(window, renderer.getStatsManager());

    liquid::rhi::RenderGraph graph;

    liquid::SceneRenderer sceneRenderer(entityContext, false);

    liquid::rhi::TextureDescription desc;
    desc.usage =
        liquid::rhi::TextureUsage::Depth | liquid::rhi::TextureUsage::Sampled;
    desc.sizeMethod = liquid::rhi::TextureSizeMethod::SwapchainRatio;
    desc.width = 100;
    desc.height = 100;
    desc.layers = 1;
    desc.format = VK_FORMAT_D32_SFLOAT;
    auto depthBuffer = renderer.getRegistry().setTexture(desc);

    auto pipeline = renderer.getRegistry().setPipeline(
        {vertexShader, fragmentShader,
         liquid::rhi::PipelineVertexInputLayout::create<liquid::Vertex>(),
         liquid::rhi::PipelineInputAssembly{
             liquid::rhi::PrimitiveTopology::TriangleList},
         liquid::rhi::PipelineRasterizer{liquid::rhi::PolygonMode::Fill,
                                         liquid::rhi::CullMode::None,
                                         liquid::rhi::FrontFace::Clockwise},
         liquid::rhi::PipelineColorBlend{
             {liquid::rhi::PipelineColorBlendAttachment{}}}});

    auto &pass = graph.addPass("mainPass");
    pass.write(graph.getSwapchain(), glm::vec4{0.0f});
    pass.write(depthBuffer, liquid::rhi::DepthStencilClear{1.0f, 0});
    pass.addPipeline(pipeline);
    pass.setExecutor([pipeline, this, &sceneRenderer](auto &commandList) {
      commandList.bindPipeline(pipeline);

      liquid::rhi::Descriptor descriptor;
      descriptor.bind(0, camera->getBuffer(),
                      liquid::rhi::DescriptorType::UniformBuffer);

      commandList.bindDescriptor(pipeline, 0, descriptor);

      sceneRenderer.render(commandList, pipeline);
    });

    mainLoop.setUpdateFn([=](float dt) mutable {
      eventSystem.poll();
      if (gameEnded) {
        auto &transform =
            entityContext.getComponent<liquid::TransformComponent>(
                ball->getEntity());
        transform.localPosition = glm::vec3{0.0f, 0.0f, 0.0f};
        gameEnded = false;
      }
      updateScene();
      physicsSystem.update(dt);
      updateGameLogic(dt);

      return true;
    });

    mainLoop.setRenderFn([this, &graph]() { renderer.render(graph); });

    mainLoop.run();
    renderer.wait();
    return 0;
  }

private:
  void handleKeyClick(int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) {
      playerVelocity = 0.0;
    }

    // TODO: Smoother input handling
    // When switching between input clicks, the input gets confused
    // We might need some kind of a queue here to make sure that multiple
    // input clicks are handled

    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
      playerVelocity = -velocity;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
      playerVelocity = velocity;
    }
  }

  void updateScene() {
    {
      auto &transform = p2->getTransform();
      transform.localPosition.x = botPosition;
    }

    {
      auto &transform = p1->getTransform();
      transform.localPosition.x = playerPosition;
    }

    scene->update();
  }

  void updateGameLogic(float dt) {

    static bool firstTime = true;
    if (firstTime) {
      auto &rigidBody = entityContext.getComponent<liquid::RigidBodyComponent>(
          ball->getEntity());
      rigidBody.actor->addForce({0.0f, 0.0f, 800.0f});
      rigidBody.actor->addTorque({10.0f, 0.0f, 0.0f});

      firstTime = !firstTime;
    }

    if (abs(botPosition - ball->getTransform().localPosition.x) < 0.2f) {
      botVelocity = 0.0;
    } else if (botPosition > ball->getTransform().localPosition.x) {
      botVelocity = -velocity;
    } else {
      botVelocity = velocity;
    }

    float newBotPosition = botPosition + botVelocity * dt;
    if (newBotPosition - paddleWidth > wallLineRight &&
        newBotPosition + paddleWidth < wallLineLeft) {
      botPosition = newBotPosition;
    }

    float newPlayerPosition = playerPosition + playerVelocity * dt;
    if (newPlayerPosition - paddleWidth > wallLineRight &&
        newPlayerPosition + paddleWidth < wallLineLeft) {
      playerPosition = newPlayerPosition;
    }
  }

  void setupScene() {
    auto cameraEntity = entityContext.createEntity();
    entityContext.setComponent<liquid::CameraComponent>(cameraEntity, {camera});
    scene->setActiveCamera(cameraEntity);

    const auto &fbSize = window.getFramebufferSize();

    camera->setPerspective(70.0, static_cast<float>(fbSize.x) / fbSize.y, 0.1f,
                           200.0f);
    camera->lookAt({0.0f, 4.0f, -8.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

    window.addResizeHandler([this](uint32_t width, uint32_t height) {
      this->camera->setPerspective(70.0f, static_cast<float>(width) / height,
                                   0.1f, 200.0f);
    });

    auto e1 = entityContext.createEntity();
    auto e2 = entityContext.createEntity();
    auto e3 = entityContext.createEntity();
    auto e4 = entityContext.createEntity();

    auto pe1 = entityContext.createEntity();
    auto pe2 = entityContext.createEntity();
    auto ballEntity = entityContext.createEntity();

    entityContext.setComponent<liquid::MeshComponent>(e1, {barInstance});
    entityContext.setComponent<liquid::MeshComponent>(e2, {barInstance});
    entityContext.setComponent<liquid::MeshComponent>(e3, {barInstance});
    entityContext.setComponent<liquid::MeshComponent>(e4, {barInstance});

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
        ballEntity, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                        liquid::PhysicsGeometryType::Sphere,
                        liquid::PhysicsGeometrySphere{ballRadius}}});

    entityContext.setComponent<liquid::RigidBodyComponent>(
        ballEntity,
        liquid::RigidBodyComponent{{1.0f, {0.05f, 100.0f, 100.0f}, false}});

    entityContext.setComponent<liquid::MeshComponent>(pe1, {barInstance});
    entityContext.setComponent<liquid::MeshComponent>(pe2, {barInstance});
    entityContext.setComponent<liquid::MeshComponent>(ballEntity,
                                                      {ballInstance});

    // Create walls
    scene->getRootNode()->addChild(
        e1, createWallTransform({0.0f, 0.0f, 3.5f}, 0.0f, 5.0f));
    scene->getRootNode()->addChild(
        e2, createWallTransform({-5.0f, 0.0f, 0.0f}, 90.0f, 3.6f));
    scene->getRootNode()->addChild(
        e3, createWallTransform({5.0f, 0.0f, 0.0f}, 90.0f, 3.6f));
    scene->getRootNode()->addChild(
        e4, createWallTransform({0.0f, 0.0f, -3.5f}, 0.0f, 5.0f));

    eventSystem.observe(
        liquid::CollisionEvent::CollisionStarted,
        [ballEntity, e1, e4, this](const liquid::CollisionObject &data) {
          if ((data.a == ballEntity || data.b == ballEntity) &&
              (data.a == e1 || data.b == e1 || data.a == e4 || data.b == e4)) {
            gameEnded = true;
          }
        });

    // Create paddles
    {
      liquid::TransformComponent transform;
      transform.localPosition = glm::vec3(playerPosition, 0.0f, -3.0f);
      transform.localScale = glm::vec3{1.0f, 0.2f, 0.1f};

      entityContext.setComponent<liquid::CollidableComponent>(
          pe1, liquid::CollidableComponent{liquid::PhysicsGeometryDesc{
                   liquid::PhysicsGeometryType::Box,
                   liquid::PhysicsGeometryBox{glm::vec3(1.0f, 0.2f, 0.1f)}}});

      p1 = scene->getRootNode()->addChild(pe1, transform);
    }

    {
      liquid::TransformComponent transform;
      transform.localPosition = glm::vec3(playerPosition, 0.0f, 3.0f);
      transform.localScale = glm::vec3{1.0f, 0.2f, 0.1f};

      entityContext.setComponent<liquid::CollidableComponent>(
          pe2, liquid::CollidableComponent{
                   liquid::PhysicsGeometryDesc{
                       liquid::PhysicsGeometryType::Box,
                       liquid::PhysicsGeometryBox{glm::vec3(1.0f, 0.2f, 0.1f)}},
                   liquid::PhysicsMaterialDesc{0.0f, 0.0f, 1.0f}});

      p2 = scene->getRootNode()->addChild(pe2, transform);
    }

    // create ball
    ball = scene->getRootNode()->addChild(ballEntity);
  }

  liquid::TransformComponent createWallTransform(glm::vec3 position,
                                                 float rotation, float scaleX) {
    liquid::TransformComponent transform{};
    transform.localPosition = position;
    transform.localRotation =
        glm::angleAxis(glm::radians(rotation), glm::vec3(0, 1, 0));

    transform.localScale = glm::vec3(scaleX, 0.2f, 0.1f);
    return transform;
  }

private:
  liquid::EventSystem eventSystem;
  liquid::EntityContext entityContext;
  liquid::Window window;
  liquid::rhi::VulkanRenderBackend backend;
  liquid::Renderer renderer;
  liquid::PhysicsSystem physicsSystem;

  liquid::SharedPtr<liquid::Camera> camera;
  std::unique_ptr<liquid::Scene> scene;

  liquid::rhi::ShaderHandle vertexShader;
  liquid::rhi::ShaderHandle fragmentShader;
  liquid::SharedPtr<liquid::Material> material;

  liquid::SharedPtr<liquid::MeshInstance<liquid::Mesh>> barInstance;
  liquid::SharedPtr<liquid::MeshInstance<liquid::Mesh>> ballInstance;

  liquid::SceneNode *p1, *p2, *ball;

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

  float playerVelocity = 0.0f;
  float playerPosition = 0.0f;

  float botVelocity = 0.0f;
  float botPosition = 0.0f;
  glm::vec3 ballPosition{0.0f, 0.0f, 0.0f};
  glm::vec3 ballVelocity{0.2f, 0.0f, 0.3f};
};

int main() {
  liquid::Engine::setAssetsPath(
      std::filesystem::path("../../../../engine/bin/Debug/assets").string());
  Game game;
  return game.run();
}
