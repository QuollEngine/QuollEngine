#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/DirectionalLight.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/skeleton/JointAttachment.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/system/SystemView.h"
#include "SceneUpdater.h"

namespace quoll {

void SceneUpdater::createSystemViewData(SystemView &view) {
  auto &db = view.scene->entityDatabase;

  view.sceneUpdater.queryTransformsWithoutParent =
      db.query_builder<LocalTransform, WorldTransform>()
          .without<Parent>()
          .build();

  view.sceneUpdater.queryTransformsWithParent =
      db.query<LocalTransform, WorldTransform, Parent>();

  view.sceneUpdater.queryCameras =
      db.query<PerspectiveLens, WorldTransform, Camera>();

  view.sceneUpdater.queryDirectionalLights =
      db.query<DirectionalLight, WorldTransform>();
}

void SceneUpdater::update(SystemView &view) {
  QUOLL_PROFILE_EVENT("SceneUpdater::update");
  updateTransforms(view);
  updateCameras(view);
  updateLights(view);
}

void SceneUpdater::updateTransforms(SystemView &view) {
  QUOLL_PROFILE_EVENT("SceneUpdater::updateTransforms");

  view.sceneUpdater.queryTransformsWithoutParent.each(
      [](LocalTransform &local, WorldTransform &world) {
        glm::mat4 identity{1.0f};
        glm::mat4 localTransform =
            glm::translate(identity, local.localPosition) *
            glm::toMat4(local.localRotation) *
            glm::scale(identity, local.localScale);

        world.worldTransform = localTransform;
      });

  view.sceneUpdater.queryTransformsWithParent.each([](flecs::entity entity,
                                                      LocalTransform &local,
                                                      WorldTransform &world,
                                                      Parent &cparent) {
    auto &parent = cparent.parent;
    auto parentTransform = parent.get_ref<WorldTransform>();

    glm::mat4 identity{1.0f};
    glm::mat4 localTransform = glm::translate(identity, local.localPosition) *
                               glm::toMat4(local.localRotation) *
                               glm::scale(identity, local.localScale);

    i16 jointId = -1;
    if (entity.has<JointAttachment>() && parent.has<Skeleton>()) {
      jointId = entity.get_ref<JointAttachment>()->joint;
    }

    if (jointId >= 0 &&
        static_cast<usize>(jointId) <
            parent.get_ref<Skeleton>()->jointWorldTransforms.size()) {
      const auto &jointTransform =
          parent.get_ref<Skeleton>()->jointWorldTransforms.at(jointId);
      world.worldTransform =
          parentTransform->worldTransform * jointTransform * localTransform;
    } else {
      world.worldTransform = parentTransform->worldTransform * localTransform;
    }
  });
}

void SceneUpdater::updateCameras(SystemView &view) {
  QUOLL_PROFILE_EVENT("SceneUpdater::updateCameras");

  view.sceneUpdater.queryCameras.each([](PerspectiveLens &lens,
                                         WorldTransform &world,
                                         Camera &camera) {
    const f32 fovY =
        2.0f * atanf(lens.sensorSize.y / (2.0f * lens.focalLength));

    camera.projectionMatrix =
        glm::perspective(fovY, lens.aspectRatio, lens.near, lens.far);

    camera.viewMatrix = glm::inverse(world.worldTransform);
    camera.projectionViewMatrix = camera.projectionMatrix * camera.viewMatrix;

    const f32 ev100 = std::log2f(powf(lens.aperture, 2.0f) * lens.shutterSpeed *
                                 100.0f / static_cast<f32>(lens.sensitivity));
    camera.exposure.x = ev100;
  });
}

void SceneUpdater::updateLights(SystemView &view) {
  QUOLL_PROFILE_EVENT("SceneUpdater::updateLights");

  view.sceneUpdater.queryDirectionalLights.each([](DirectionalLight &light,
                                                   WorldTransform &world) {
    glm::quat rotation;
    glm::vec3 empty3;
    glm::vec4 empty4;
    glm::vec3 position;

    glm::decompose(world.worldTransform, empty3, rotation, position, empty3,
                   empty4);

    light.direction =
        glm::normalize(glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
  });
}

} // namespace quoll
