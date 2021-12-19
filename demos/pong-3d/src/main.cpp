#include "core/Base.h"
#include "core/Engine.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "renderer/vulkan/VulkanRenderer.h"
#include "window/glfw/GLFWWindow.h"

#include "scene/Vertex.h"
#include "scene/Mesh.h"
#include "scene/MeshInstance.h"
#include "scene/Camera.h"
#include "scene/Scene.h"

#include "loop/MainLoop.h"

#include "cube.h"
#include "sphere.h"

class Game {
public:
  Game()
      : window(new liquid::GLFWWindow("Pong 3D", 800, 600)),
        renderer(new liquid::VulkanRenderer(entityContext, window.get(), true)),
        vertexShader(renderer->createShader("basic-shader.vert.spv")),
        fragmentShader(renderer->createShader("basic-shader.frag.spv")),
        material(renderer->createMaterial(vertexShader, fragmentShader, {}, {},
                                          liquid::CullMode::None)),
        camera(new liquid::Camera(renderer->getResourceAllocator())) {

    scene.reset(new liquid::Scene(entityContext));

    liquid::Mesh barMesh = createCube();
    liquid::Mesh ballMesh = createSphere(ballRadius, 10, 10, RED);

    window->addKeyHandler([this](int key, int scancode, int action, int mods) {
      handleKeyClick(key, scancode, action, mods);
    });

    barInstance.reset(
        new liquid::MeshInstance(&barMesh, renderer->getResourceAllocator()));
    barInstance->setMaterial(material);

    ballInstance.reset(
        new liquid::MeshInstance(&ballMesh, renderer->getResourceAllocator()));
    ballInstance->setMaterial(material);

    setupScene();
  }

  int run() {

    liquid::MainLoop mainLoop(renderer.get(), window.get());
    const auto &renderData = renderer->prepareScene(scene.get());
    auto graph = renderer->createRenderGraph(renderData);

    try {
      return mainLoop.run(
          graph,
          [=](double dt) mutable {
            updateGameLogic(0.15);
            updateScene(0.15);

            return true;
          },
          []() {});

    } catch (std::runtime_error error) {
      std::cerr << error.what() << std::endl;
      return 1;
    }
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

  void updateScene(float dt) {
    p2->setTransform(glm::translate(glm::mat4{1.0f}, {botPosition, 0.0, 3.0}) *
                     glm::scale(glm::mat4{1.0}, {1.0, 0.2, 0.1}));

    p1->setTransform(
        glm::translate(glm::mat4{1.0f}, {playerPosition, 0.0, -3.0}) *
        glm::scale(glm::mat4{1.0}, {1.0, 0.2, 0.1}));

    ball->setTransform(glm::translate(
        glm::mat4{1.0f}, {ballPosition.x, ballPosition.y, ballPosition.z}));

    scene->update();
  }

  void updateGameLogic(float dt) {
    if (abs(botPosition - ballPosition.x) < 0.2) {
      botVelocity = 0;
    } else if (botPosition > ballPosition.x) {
      botVelocity = -velocity;
    } else {
      botVelocity = velocity;
    }

    // TODO: Better movement in x direction
    // This "easy" method has caveats and it would be
    // better to calculate position based on incoming angle
    // and output it based on the angle
    // TODO: Changing Z velocity for more fun
    if (ballPosition.z > safeAreaTop) {
      if ((ballPosition.x - ballRadius - botPosition - paddleWidth < 0.1) &&
          (ballPosition.x + ballRadius - botPosition + paddleWidth) > -0.1) {
        ballVelocity.z = -ballVelocity.z;
        ballVelocity.x *= 1.04;

        if ((ballPosition.x - playerPosition) * ballVelocity.x <= 0) {
          ballVelocity.x *= -1;
        }
      } else {
        ballPosition = {0, 0, 0};
        ballVelocity = {0.1, 0.0, 0.3};
      }
    } else if (ballPosition.z < safeAreaBottom) {
      if ((ballPosition.x - ballRadius - playerPosition - paddleWidth < 0.1) &&
          (ballPosition.x + ballRadius - playerPosition + paddleWidth) > -0.1) {
        ballVelocity.z = -ballVelocity.z;

        ballVelocity.x *= 1.04;

        if ((ballPosition.x - playerPosition) * ballVelocity.x <= 0) {
          ballVelocity.x *= -1;
        }
      } else {
        ballPosition = {0, 0, 0};
        ballVelocity = {0.1, 0.0, -0.3};
      }
    }

    if (ballPosition.x + ballRadius > wallLineLeft ||
        ballPosition.x - ballRadius < wallLineRight) {
      ballVelocity.x *= -1;
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

    ballPosition += ballVelocity * dt;
  }

  void setupScene() {

    auto cameraEntity = entityContext.createEntity();
    entityContext.setComponent<liquid::CameraComponent>(cameraEntity, {camera});
    scene->setActiveCamera(cameraEntity);

    const auto &fbSize = window->getFramebufferSize();

    camera->setPerspective(
        70.0, static_cast<float>(fbSize.width) / fbSize.height, 0.1, 200.0);
    camera->lookAt({0.0, 4.0, -8.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0});

    window->addResizeHandler([this](uint32_t width, uint32_t height) {
      this->camera->setPerspective(70.0, static_cast<float>(width) / height,
                                   0.1, 200.0);
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

    entityContext.setComponent<liquid::MeshComponent>(pe1, {barInstance});
    entityContext.setComponent<liquid::MeshComponent>(pe2, {barInstance});
    entityContext.setComponent<liquid::MeshComponent>(ballEntity,
                                                      {ballInstance});

    // Create walls
    scene->getRootNode()->addChild(
        e1, createWallTransform({0.0, 0.0, 3.5}, 0, 5.0));
    scene->getRootNode()->addChild(
        e2, createWallTransform({-5.0, 0.0, 0}, 90, 3.6));
    scene->getRootNode()->addChild(e3,
                                   createWallTransform({5.0, 0.0, 0}, 90, 3.6));
    scene->getRootNode()->addChild(
        e4, createWallTransform({0.0, 0.0, -3.5}, 0, 5.0));

    // Create paddles
    p1 = scene->getRootNode()->addChild(pe1, glm::mat4());
    p2 = scene->getRootNode()->addChild(pe2, glm::mat4());

    // create ball
    ball = scene->getRootNode()->addChild(ballEntity, glm::mat4());
  }

  glm::mat4 createWallTransform(glm::vec3 position, float rotation,
                                float scaleX) {
    return glm::translate(glm::mat4{1.0}, position) *
           glm::rotate(glm::mat4{1.0}, glm::radians(rotation),
                       glm::vec3(0, 1, 0)) *
           glm::scale(glm::mat4{1.0}, {scaleX, 0.2, 0.1});
  }

private:
  std::unique_ptr<liquid::GLFWWindow> window;
  std::unique_ptr<liquid::VulkanRenderer> renderer;
  liquid::SharedPtr<liquid::Camera> camera;
  std::unique_ptr<liquid::Scene> scene;

  liquid::SharedPtr<liquid::Shader> vertexShader;
  liquid::SharedPtr<liquid::Shader> fragmentShader;
  liquid::SharedPtr<liquid::Material> material;

  liquid::SharedPtr<liquid::MeshInstance> barInstance;
  liquid::SharedPtr<liquid::MeshInstance> ballInstance;

  liquid::SceneNode *p1, *p2, *ball;

private:
  // Game specific parameters
  float velocity = 0.4f;

  float ballRadius = 0.3;

  // wall center position - width
  float wallLineLeft = 5.0 - 0.1;
  float wallLineRight = -5 + 0.1;

  // cube width / 2
  float paddleWidth = 1.0;

  float safeAreaTop = 2.7;
  float safeAreaBottom = -2.7;

  float playerVelocity = 0;
  float playerPosition = 0;

  float botVelocity = 0.0;
  float botPosition = 0.0;
  glm::vec3 ballPosition{0.0, 0.0, 0.0};
  glm::vec3 ballVelocity{0.2, 0.0, 0.3};

  liquid::EntityContext entityContext;
};

int main() {
  liquid::Engine::setAssetsPath(
      std::filesystem::path("../../../../engine/bin/Debug/assets").string());
  Game game;
  return game.run();
}
