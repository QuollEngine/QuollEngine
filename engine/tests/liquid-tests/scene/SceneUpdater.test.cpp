#include "liquid/core/Base.h"
#include "liquid/scene/SceneUpdater.h"

#include "liquid-tests/Testing.h"

class SceneUpdaterTest : public ::testing::Test {
public:
  quoll::EntityDatabase entityDatabase;
  quoll::SceneUpdater sceneUpdater;
};

glm::mat4 getLocalTransform(const quoll::LocalTransform &transform) {
  return glm::translate(glm::mat4(1.0f), transform.localPosition) *
         glm::toMat4(transform.localRotation) *
         glm::scale(glm::mat4(1.0f), transform.localScale);
}

TEST_F(SceneUpdaterTest, SetsLocalTransformToWorldTransformIfNoParent) {
  auto entity = entityDatabase.create();
  quoll::LocalTransform transform{};
  transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);

  entityDatabase.set<quoll::WorldTransform>(entity, {});
  entityDatabase.set(entity, transform);

  sceneUpdater.update(entityDatabase);

  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(entity).worldTransform,
            getLocalTransform(transform));
}

TEST_F(SceneUpdaterTest, CalculatesWorldTransformFromParentWorldTransform) {
  // parent
  auto parent = entityDatabase.create();
  quoll::LocalTransform parentTransform{};
  parentTransform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  parentTransform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  parentTransform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(parent, parentTransform);
  entityDatabase.set<quoll::WorldTransform>(parent, {});

  // parent -> child1
  auto child1 = entityDatabase.create();
  quoll::LocalTransform child1Transform{};
  child1Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child1Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child1Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(child1, child1Transform);
  entityDatabase.set<quoll::Parent>(child1, {parent});
  entityDatabase.set<quoll::WorldTransform>(child1, {});

  // parent -> child1 -> child2
  auto child2 = entityDatabase.create();
  quoll::LocalTransform child2Transform{};
  child2Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child2Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child2Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(child2, child2Transform);
  entityDatabase.set<quoll::Parent>(child2, {child1});
  entityDatabase.set<quoll::WorldTransform>(child2, {});

  sceneUpdater.update(entityDatabase);

  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(parent).worldTransform,
            getLocalTransform(parentTransform));

  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(child1).worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform));

  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(child2).worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform) *
                getLocalTransform(child2Transform));
}

TEST_F(SceneUpdaterTest,
       CalculatesWorldBasedOnParentIfJointAttachmentIsInvalid) {
  // parent
  auto parent = entityDatabase.create();
  quoll::LocalTransform parentTransform{};
  parentTransform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  parentTransform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  parentTransform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(parent, parentTransform);
  entityDatabase.set<quoll::WorldTransform>(parent, {});

  glm::vec3 jointPosition = glm::vec3{4.0f, 5.0f, 6.0f};
  quoll::Skeleton skeleton;
  skeleton.jointWorldTransforms.push_back({});
  skeleton.jointWorldTransforms.push_back(
      glm::translate(glm::mat4{1.0f}, glm::vec3{4.0f, 5.0f, 6.0f}));
  entityDatabase.set(parent, skeleton);

  // parent -> child1
  // Joint Id must be positive
  auto child1 = entityDatabase.create();
  quoll::LocalTransform child1Transform{};
  child1Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child1Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child1Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(child1, child1Transform);
  entityDatabase.set<quoll::Parent>(child1, {parent});
  entityDatabase.set<quoll::WorldTransform>(child1, {});
  entityDatabase.set<quoll::JointAttachment>(child1, {-1});

  // parent -> child2
  // Joint ID must be within the range of skeleton
  auto child2 = entityDatabase.create();
  quoll::LocalTransform child2Transform{};
  child2Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child2Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child2Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(child2, child2Transform);
  entityDatabase.set<quoll::Parent>(child2, {parent});
  entityDatabase.set<quoll::WorldTransform>(child2, {});
  entityDatabase.set<quoll::JointAttachment>(child2, {2});

  // parent -> child2 -> child3
  // Joint attachment entity must be **immediate**
  // child of a skeleton entity
  auto child3 = entityDatabase.create();
  quoll::LocalTransform child3Transform{};
  child3Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child3Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child3Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(child3, child3Transform);
  entityDatabase.set<quoll::Parent>(child3, {child2});
  entityDatabase.set<quoll::WorldTransform>(child3, {});
  entityDatabase.set<quoll::JointAttachment>(child3, {1});

  sceneUpdater.update(entityDatabase);

  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(parent).worldTransform,
            getLocalTransform(parentTransform));
  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(child1).worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform));
  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(child2).worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child2Transform));
  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(child3).worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child2Transform) *
                getLocalTransform(child3Transform));
}

TEST_F(SceneUpdaterTest,
       CalculatesWorldTransformBasedBasedOnJointAttachmentAndParent) {
  // parent
  auto parent = entityDatabase.create();
  quoll::LocalTransform parentTransform{};
  parentTransform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  parentTransform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  parentTransform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(parent, parentTransform);
  entityDatabase.set<quoll::WorldTransform>(parent, {});

  // parent -> child1
  auto child1 = entityDatabase.create();
  quoll::LocalTransform child1Transform{};
  child1Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child1Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child1Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(child1, child1Transform);
  entityDatabase.set<quoll::Parent>(child1, {parent});
  entityDatabase.set<quoll::WorldTransform>(child1, {});

  glm::vec3 jointPosition = glm::vec3{4.0f, 5.0f, 6.0f};
  quoll::Skeleton skeleton;
  skeleton.jointWorldTransforms.push_back({});
  skeleton.jointWorldTransforms.push_back(
      glm::translate(glm::mat4{1.0f}, glm::vec3{4.0f, 5.0f, 6.0f}));
  entityDatabase.set(child1, skeleton);

  // parent -> child1 -> child2
  auto child2 = entityDatabase.create();
  quoll::LocalTransform child2Transform{};
  child2Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child2Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child2Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  entityDatabase.set(child2, child2Transform);
  entityDatabase.set<quoll::Parent>(child2, {child1});
  entityDatabase.set<quoll::WorldTransform>(child2, {});

  // Set second joint as attachment
  entityDatabase.set<quoll::JointAttachment>(child2, {1});

  sceneUpdater.update(entityDatabase);

  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(parent).worldTransform,
            getLocalTransform(parentTransform));

  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(child1).worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform));

  EXPECT_EQ(entityDatabase.get<quoll::WorldTransform>(child2).worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform) *
                getLocalTransform({jointPosition}) *
                getLocalTransform(child2Transform));
}

TEST_F(SceneUpdaterTest, UpdatesCameraBasedOnTransformAndPerspectiveLens) {
  auto entity = entityDatabase.create();

  {
    quoll::LocalTransform transform{};
    transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
    entityDatabase.set(entity, transform);
    entityDatabase.set<quoll::WorldTransform>(entity, {});

    quoll::PerspectiveLens lens{};
    entityDatabase.set(entity, lens);

    quoll::Camera camera{};
    entityDatabase.set(entity, camera);
  }
  sceneUpdater.update(entityDatabase);

  auto &transform = entityDatabase.get<quoll::WorldTransform>(entity);
  auto &lens = entityDatabase.get<quoll::PerspectiveLens>(entity);
  auto &camera = entityDatabase.get<quoll::Camera>(entity);

  float fovY = 2.0f * atanf(lens.sensorSize.y / (2.0f * lens.focalLength));

  auto expectedPerspective =
      glm::perspective(fovY, lens.aspectRatio, lens.near, lens.far);

  EXPECT_EQ(camera.viewMatrix, glm::inverse(transform.worldTransform));
  EXPECT_EQ(camera.projectionMatrix, expectedPerspective);
  EXPECT_EQ(camera.projectionViewMatrix,
            camera.projectionMatrix * camera.viewMatrix);
  EXPECT_NEAR(camera.exposure.x, -0.965f, 0.001f);
}

TEST_F(SceneUpdaterTest, UpdateDirectionalLightsBasedOnTransforms) {
  auto entity = entityDatabase.create();

  {
    quoll::LocalTransform transform{};
    transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
    entityDatabase.set(entity, transform);
    entityDatabase.set<quoll::WorldTransform>(entity, {});

    quoll::DirectionalLight light{};
    entityDatabase.set(entity, light);
  }
  sceneUpdater.update(entityDatabase);

  auto &transform = entityDatabase.get<quoll::WorldTransform>(entity);
  auto &light = entityDatabase.get<quoll::DirectionalLight>(entity);

  glm::quat rotation;
  glm::vec3 empty3;
  glm::vec4 empty4;
  glm::vec3 position;

  glm::decompose(transform.worldTransform, empty3, rotation, position, empty3,
                 empty4);

  auto expected =
      glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));

  EXPECT_EQ(light.direction, expected);
}
