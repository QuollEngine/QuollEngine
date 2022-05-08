#include "liquid/core/Base.h"
#include "liquid/scene/Scene.h"

#include <gtest/gtest.h>

class SceneTest : public ::testing::Test {
public:
  liquid::rhi::ResourceRegistry registry;
  liquid::EntityContext context;
};

using SceneDeathTest = SceneTest;

TEST_F(SceneTest, CreatesEmptyRootNodeOnConstruct) {
  liquid::Scene scene(context);

  auto *rootNode = scene.getRootNode();

  scene.update();

  EXPECT_NE(rootNode->getEntity(), std::numeric_limits<liquid::Entity>::max());
  EXPECT_EQ(rootNode->getParent(), nullptr);
  EXPECT_TRUE(rootNode->getWorldTransform() == glm::mat4{1.0f});
}

TEST_F(SceneTest, UpdatesCameraBasedOnTransformAndPerspectiveLens) {
  liquid::Scene scene(context);
  auto entity = context.createEntity();

  {
    liquid::TransformComponent transform{};
    transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);

    liquid::PerspectiveLensComponent lens{};
    liquid::CameraComponent camera{};

    context.setComponent(entity, lens);
    context.setComponent(entity, camera);

    scene.getRootNode()->addChild(entity, transform);
  }
  scene.update();

  auto &transform = context.getComponent<liquid::TransformComponent>(entity);
  auto &lens = context.getComponent<liquid::PerspectiveLensComponent>(entity);
  auto &camera = context.getComponent<liquid::CameraComponent>(entity);

  auto expectedPerspective =
      glm::perspective(lens.fovY, lens.aspectRatio, lens.near, lens.far);
  expectedPerspective[1][1] *= -1.0f;

  EXPECT_EQ(camera.viewMatrix, glm::inverse(transform.worldTransform));
  EXPECT_EQ(camera.projectionMatrix, expectedPerspective);
  EXPECT_EQ(camera.projectionViewMatrix,
            camera.projectionMatrix * camera.viewMatrix);
}

TEST_F(SceneTest, UpdateDirectionalLightsBasedOnTransforms) {
  liquid::Scene scene(context);
  auto entity = context.createEntity();

  {
    liquid::TransformComponent transform{};
    transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);

    liquid::DirectionalLightComponent light{};
    context.setComponent(entity, light);

    scene.getRootNode()->addChild(entity, transform);
  }
  scene.update();

  auto &transform = context.getComponent<liquid::TransformComponent>(entity);
  auto &light = context.getComponent<liquid::DirectionalLightComponent>(entity);

  glm::quat rotation;
  glm::vec3 empty3;
  glm::vec4 empty4;
  glm::vec3 position;

  glm::decompose(transform.worldTransform, empty3, rotation, position, empty3,
                 empty4);

  rotation = glm::conjugate(rotation);
  auto expected =
      glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));

  EXPECT_EQ(light.direction, expected);
}
