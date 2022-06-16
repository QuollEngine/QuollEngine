#include "liquid/core/Base.h"
#include "liquid/scene/SceneUpdater.h"

#include "liquid-tests/Testing.h"

class SceneUpdaterTest : public ::testing::Test {
public:
  liquid::EntityContext context;
  liquid::SceneUpdater sceneUpdater;
};

glm::mat4 getLocalTransform(const liquid::LocalTransformComponent &transform) {
  return glm::translate(glm::mat4(1.0f), transform.localPosition) *
         glm::toMat4(transform.localRotation) *
         glm::scale(glm::mat4(1.0f), transform.localScale);
}

TEST_F(SceneUpdaterTest, SetsLocalTransformToWorldTransformIfNoParent) {
  auto entity = context.createEntity();
  liquid::LocalTransformComponent transform{};
  transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);

  context.setComponent<liquid::WorldTransformComponent>(entity, {});
  context.setComponent(entity, transform);

  sceneUpdater.update(context);

  EXPECT_EQ(context.getComponent<liquid::WorldTransformComponent>(entity)
                .worldTransform,
            getLocalTransform(transform));
}

TEST_F(SceneUpdaterTest, CalculatesWorldTransformFromParentWorldTransform) {
  // parent
  auto parent = context.createEntity();
  liquid::LocalTransformComponent parentTransform{};
  parentTransform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  parentTransform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  parentTransform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  context.setComponent(parent, parentTransform);
  context.setComponent<liquid::WorldTransformComponent>(parent, {});

  // parent -> child1
  auto child1 = context.createEntity();
  liquid::LocalTransformComponent child1Transform{};
  child1Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child1Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child1Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  context.setComponent(child1, child1Transform);
  context.setComponent<liquid::ParentComponent>(child1, {parent});
  context.setComponent<liquid::WorldTransformComponent>(child1, {});

  // parent -> child1 -> child2
  auto child2 = context.createEntity();
  liquid::LocalTransformComponent child2Transform{};
  child2Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child2Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child2Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  context.setComponent(child2, child2Transform);
  context.setComponent<liquid::ParentComponent>(child2, {child1});
  context.setComponent<liquid::WorldTransformComponent>(child2, {});

  sceneUpdater.update(context);

  EXPECT_EQ(context.getComponent<liquid::WorldTransformComponent>(parent)
                .worldTransform,
            getLocalTransform(parentTransform));

  EXPECT_EQ(context.getComponent<liquid::WorldTransformComponent>(child1)
                .worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform));

  EXPECT_EQ(context.getComponent<liquid::WorldTransformComponent>(child2)
                .worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform) *
                getLocalTransform(child2Transform));
}

TEST_F(SceneUpdaterTest, UpdatesCameraBasedOnTransformAndPerspectiveLens) {
  auto entity = context.createEntity();

  {
    liquid::LocalTransformComponent transform{};
    transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
    context.setComponent(entity, transform);
    context.setComponent<liquid::WorldTransformComponent>(entity, {});

    liquid::PerspectiveLensComponent lens{};
    context.setComponent(entity, lens);

    liquid::CameraComponent camera{};
    context.setComponent(entity, camera);
  }
  sceneUpdater.update(context);

  auto &transform =
      context.getComponent<liquid::WorldTransformComponent>(entity);
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

TEST_F(SceneUpdaterTest, UpdateDirectionalLightsBasedOnTransforms) {
  auto entity = context.createEntity();

  {
    liquid::LocalTransformComponent transform{};
    transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
    context.setComponent(entity, transform);
    context.setComponent<liquid::WorldTransformComponent>(entity, {});

    liquid::DirectionalLightComponent light{};
    context.setComponent(entity, light);
  }
  sceneUpdater.update(context);

  auto &transform =
      context.getComponent<liquid::WorldTransformComponent>(entity);
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
