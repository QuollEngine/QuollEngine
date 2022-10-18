#include "liquid/core/Base.h"
#include "liquid/scene/SceneUpdater.h"

#include "liquid-tests/Testing.h"

class SceneUpdaterTest : public ::testing::Test {
public:
  liquid::EntityDatabase entityDatabase;
  liquid::SceneUpdater sceneUpdater;
};

glm::mat4 getLocalTransform(const liquid::LocalTransformComponent &transform) {
  return glm::translate(glm::mat4(1.0f), transform.localPosition) *
         glm::toMat4(transform.localRotation) *
         glm::scale(glm::mat4(1.0f), transform.localScale);
}

TEST_F(SceneUpdaterTest, SetsLocalTransformToWorldTransformIfNoParent) {
  auto entity = entityDatabase.create();
  liquid::LocalTransformComponent transform{};
  transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);

  entityDatabase.set<liquid::WorldTransformComponent>(entity, {});
  entityDatabase.set(entity, transform);

  sceneUpdater.update(entityDatabase);

  EXPECT_EQ(entityDatabase.get<liquid::WorldTransformComponent>(entity)
                .worldTransform,
            getLocalTransform(transform));
}

TEST_F(SceneUpdaterTest, CalculatesWorldTransformFromParentWorldTransform) {
  // parent
  auto parent = entityDatabase.create();
  liquid::LocalTransformComponent parentTransform{};
  parentTransform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  parentTransform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  parentTransform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(parent, parentTransform);
  entityDatabase.set<liquid::WorldTransformComponent>(parent, {});

  // parent -> child1
  auto child1 = entityDatabase.create();
  liquid::LocalTransformComponent child1Transform{};
  child1Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child1Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child1Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(child1, child1Transform);
  entityDatabase.set<liquid::ParentComponent>(child1, {parent});
  entityDatabase.set<liquid::WorldTransformComponent>(child1, {});

  // parent -> child1 -> child2
  auto child2 = entityDatabase.create();
  liquid::LocalTransformComponent child2Transform{};
  child2Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child2Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child2Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(child2, child2Transform);
  entityDatabase.set<liquid::ParentComponent>(child2, {child1});
  entityDatabase.set<liquid::WorldTransformComponent>(child2, {});

  sceneUpdater.update(entityDatabase);

  EXPECT_EQ(entityDatabase.get<liquid::WorldTransformComponent>(parent)
                .worldTransform,
            getLocalTransform(parentTransform));

  EXPECT_EQ(entityDatabase.get<liquid::WorldTransformComponent>(child1)
                .worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform));

  EXPECT_EQ(entityDatabase.get<liquid::WorldTransformComponent>(child2)
                .worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform) *
                getLocalTransform(child2Transform));
}

TEST_F(SceneUpdaterTest, UpdatesCameraBasedOnTransformAndPerspectiveLens) {
  auto entity = entityDatabase.create();

  {
    liquid::LocalTransformComponent transform{};
    transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
    entityDatabase.set(entity, transform);
    entityDatabase.set<liquid::WorldTransformComponent>(entity, {});

    liquid::PerspectiveLensComponent lens{};
    entityDatabase.set(entity, lens);

    liquid::CameraComponent camera{};
    entityDatabase.set(entity, camera);
  }
  sceneUpdater.update(entityDatabase);

  auto &transform = entityDatabase.get<liquid::WorldTransformComponent>(entity);
  auto &lens = entityDatabase.get<liquid::PerspectiveLensComponent>(entity);
  auto &camera = entityDatabase.get<liquid::CameraComponent>(entity);

  auto expectedPerspective = glm::perspective(
      glm::radians(lens.fovY), lens.aspectRatio, lens.near, lens.far);

  EXPECT_EQ(camera.viewMatrix, glm::inverse(transform.worldTransform));
  EXPECT_EQ(camera.projectionMatrix, expectedPerspective);
  EXPECT_EQ(camera.projectionViewMatrix,
            camera.projectionMatrix * camera.viewMatrix);
}

TEST_F(SceneUpdaterTest, UpdateDirectionalLightsBasedOnTransforms) {
  auto entity = entityDatabase.create();

  {
    liquid::LocalTransformComponent transform{};
    transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
    entityDatabase.set(entity, transform);
    entityDatabase.set<liquid::WorldTransformComponent>(entity, {});

    liquid::DirectionalLightComponent light{};
    entityDatabase.set(entity, light);
  }
  sceneUpdater.update(entityDatabase);

  auto &transform = entityDatabase.get<liquid::WorldTransformComponent>(entity);
  auto &light = entityDatabase.get<liquid::DirectionalLightComponent>(entity);

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
