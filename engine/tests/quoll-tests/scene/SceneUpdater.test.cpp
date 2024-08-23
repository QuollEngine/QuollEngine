#include "quoll/core/Base.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/DirectionalLight.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/PointLight.h"
#include "quoll/scene/Scene.h"
#include "quoll/scene/SceneUpdater.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/skeleton/JointAttachment.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/system/SystemView.h"
#include "quoll-tests/Testing.h"

class SceneUpdaterTest : public ::testing::Test {
public:
  SceneUpdaterTest() { sceneUpdater.createSystemViewData(view); }

  quoll::Scene scene;
  quoll::EntityDatabase &entityDatabase = scene.entityDatabase;
  quoll::SystemView view{&scene};

  quoll::SceneUpdater sceneUpdater;
};

glm::mat4 getLocalTransform(const quoll::LocalTransform &transform) {
  return glm::translate(glm::mat4(1.0f), transform.localPosition) *
         glm::toMat4(transform.localRotation) *
         glm::scale(glm::mat4(1.0f), transform.localScale);
}

TEST_F(SceneUpdaterTest, SetsLocalTransformToWorldTransformIfNoParent) {
  auto entity = entityDatabase.entity();
  quoll::LocalTransform transform{};
  transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);

  entity.set<quoll::WorldTransform>({});
  entity.set(transform);

  sceneUpdater.update(view);

  EXPECT_EQ(entity.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(transform));
}

TEST_F(SceneUpdaterTest, CalculatesWorldTransformFromParentWorldTransform) {
  // parent
  auto parent = entityDatabase.entity();
  quoll::LocalTransform parentTransform{};
  parentTransform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  parentTransform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  parentTransform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  parent.set(parentTransform);
  parent.set<quoll::WorldTransform>({});

  // parent -> child1
  auto child1 = entityDatabase.entity();
  quoll::LocalTransform child1Transform{};
  child1Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child1Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child1Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  child1.set(child1Transform);
  child1.set<quoll::Parent>({parent});
  child1.set<quoll::WorldTransform>({});

  // parent -> child1 -> child2
  auto child2 = entityDatabase.entity();
  quoll::LocalTransform child2Transform{};
  child2Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child2Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child2Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  child2.set(child2Transform);
  child2.set<quoll::Parent>({child1});
  child2.set<quoll::WorldTransform>({});

  sceneUpdater.update(view);

  EXPECT_EQ(parent.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(parentTransform));

  EXPECT_EQ(child1.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform));

  EXPECT_EQ(child2.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform) *
                getLocalTransform(child2Transform));
}

TEST_F(SceneUpdaterTest,
       CalculatesWorldBasedOnParentIfJointAttachmentIsInvalid) {
  // parent
  auto parent = entityDatabase.entity();
  quoll::LocalTransform parentTransform{};
  parentTransform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  parentTransform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  parentTransform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  parent.set(parentTransform);
  parent.set<quoll::WorldTransform>({});

  glm::vec3 jointPosition = glm::vec3{4.0f, 5.0f, 6.0f};
  quoll::Skeleton skeleton;
  skeleton.jointWorldTransforms.push_back({});
  skeleton.jointWorldTransforms.push_back(
      glm::translate(glm::mat4{1.0f}, glm::vec3{4.0f, 5.0f, 6.0f}));
  parent.set(skeleton);

  // parent -> child1
  // Joint Id must be positive
  auto child1 = entityDatabase.entity();
  quoll::LocalTransform child1Transform{};
  child1Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child1Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child1Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  child1.set(child1Transform);
  child1.set<quoll::Parent>({parent});
  child1.set<quoll::WorldTransform>({});
  child1.set<quoll::JointAttachment>({-1});

  // parent -> child2
  // Joint ID must be within the range of skeleton
  auto child2 = entityDatabase.entity();
  quoll::LocalTransform child2Transform{};
  child2Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child2Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child2Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  child2.set(child2Transform);
  child2.set<quoll::Parent>({parent});
  child2.set<quoll::WorldTransform>({});
  child2.set<quoll::JointAttachment>({2});

  // parent -> child2 -> child3
  // Joint attachment entity must be **immediate**
  // child of a skeleton entity
  auto child3 = entityDatabase.entity();
  quoll::LocalTransform child3Transform{};
  child3Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child3Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child3Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  child3.set(child3Transform);
  child3.set<quoll::Parent>({child2});
  child3.set<quoll::WorldTransform>({});
  child3.set<quoll::JointAttachment>({1});

  sceneUpdater.update(view);

  EXPECT_EQ(parent.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(parentTransform));
  EXPECT_EQ(child1.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform));
  EXPECT_EQ(child2.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child2Transform));
  EXPECT_EQ(child3.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child2Transform) *
                getLocalTransform(child3Transform));
}

TEST_F(SceneUpdaterTest,
       CalculatesWorldTransformBasedBasedOnJointAttachmentAndParent) {
  // parent
  auto parent = entityDatabase.entity();
  quoll::LocalTransform parentTransform{};
  parentTransform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  parentTransform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  parentTransform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  parent.set(parentTransform);
  parent.set<quoll::WorldTransform>({});

  // parent -> child1
  auto child1 = entityDatabase.entity();
  quoll::LocalTransform child1Transform{};
  child1Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child1Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child1Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  child1.set(child1Transform);
  child1.set<quoll::Parent>({parent});
  child1.set<quoll::WorldTransform>({});

  glm::vec3 jointPosition = glm::vec3{4.0f, 5.0f, 6.0f};
  quoll::Skeleton skeleton;
  skeleton.jointWorldTransforms.push_back({});
  skeleton.jointWorldTransforms.push_back(
      glm::translate(glm::mat4{1.0f}, glm::vec3{4.0f, 5.0f, 6.0f}));
  child1.set(skeleton);

  // parent -> child1 -> child2
  auto child2 = entityDatabase.entity();
  quoll::LocalTransform child2Transform{};
  child2Transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
  child2Transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
  child2Transform.localScale = glm::vec3(0.2f, 0.5f, 1.5f);
  child2.set(child2Transform);
  child2.set<quoll::Parent>({child1});
  child2.set<quoll::WorldTransform>({});

  // Set second joint as attachment
  child2.set<quoll::JointAttachment>({1});

  sceneUpdater.update(view);

  EXPECT_EQ(parent.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(parentTransform));

  EXPECT_EQ(child1.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform));

  EXPECT_EQ(child2.get_ref<quoll::WorldTransform>()->worldTransform,
            getLocalTransform(parentTransform) *
                getLocalTransform(child1Transform) *
                getLocalTransform({jointPosition}) *
                getLocalTransform(child2Transform));
}

TEST_F(SceneUpdaterTest, UpdatesCameraBasedOnTransformAndPerspectiveLens) {
  auto entity = entityDatabase.entity();

  {
    quoll::LocalTransform transform{};
    transform.localPosition = glm::vec3(1.0f, 0.5f, 2.5f);
    entity.set(transform);
    entity.set<quoll::WorldTransform>({});

    quoll::PerspectiveLens lens{};
    entity.set(lens);

    quoll::Camera camera{};
    entity.set(camera);
  }
  sceneUpdater.update(view);

  auto transform = entity.get_ref<quoll::WorldTransform>();
  auto lens = entity.get_ref<quoll::PerspectiveLens>();
  auto camera = entity.get_ref<quoll::Camera>();

  f32 fovY = 2.0f * atanf(lens->sensorSize.y / (2.0f * lens->focalLength));

  auto expectedPerspective =
      glm::perspective(fovY, lens->aspectRatio, lens->near, lens->far);

  EXPECT_EQ(camera->viewMatrix, glm::inverse(transform->worldTransform));
  EXPECT_EQ(camera->projectionMatrix, expectedPerspective);
  EXPECT_EQ(camera->projectionViewMatrix,
            camera->projectionMatrix * camera->viewMatrix);
  EXPECT_NEAR(camera->exposure.x, -0.965f, 0.001f);
}

TEST_F(SceneUpdaterTest, UpdateDirectionalLightsBasedOnTransforms) {
  auto entity = entityDatabase.entity();

  {
    quoll::LocalTransform transform{};
    transform.localRotation = glm::quat(-0.361f, 0.697f, -0.391f, 0.481f);
    entity.set(transform);
    entity.set<quoll::WorldTransform>({});

    quoll::DirectionalLight light{};
    entity.set(light);
  }
  sceneUpdater.update(view);

  auto transform = entity.get_ref<quoll::WorldTransform>();
  auto light = entity.get_ref<quoll::DirectionalLight>();

  glm::quat rotation;
  glm::vec3 empty3;
  glm::vec4 empty4;
  glm::vec3 position;

  glm::decompose(transform->worldTransform, empty3, rotation, position, empty3,
                 empty4);

  auto expected =
      glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));

  EXPECT_EQ(light->direction, expected);
}
