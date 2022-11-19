#include "liquid/core/Base.h"
#include "SceneUpdater.h"

namespace liquid {

void SceneUpdater::update(EntityDatabase &entityDatabase) {
  LIQUID_PROFILE_EVENT("SceneUpdater::update");
  updateTransforms(entityDatabase);
  updateCameras(entityDatabase);
  updateLights(entityDatabase);
}

void SceneUpdater::updateTransforms(EntityDatabase &entityDatabase) {
  LIQUID_PROFILE_EVENT("SceneUpdater::updateTransforms");

  entityDatabase.iterateEntities<LocalTransform, WorldTransform>(
      [this, &entityDatabase](auto entity, LocalTransform &local,
                              WorldTransform &world) {
        if (entityDatabase.has<Parent>(entity))
          return;

        glm::mat4 identity{1.0f};
        glm::mat4 localTransform =
            glm::translate(identity, local.localPosition) *
            glm::toMat4(local.localRotation) *
            glm::scale(identity, local.localScale);

        world.worldTransform = localTransform;
      });

  entityDatabase.iterateEntities<LocalTransform, WorldTransform, Parent>(
      [this, &entityDatabase](auto entity, LocalTransform &local,
                              WorldTransform &world, const Parent &parent) {
        auto &parentTransform =
            entityDatabase.get<WorldTransform>(parent.parent);

        glm::mat4 identity{1.0f};
        glm::mat4 localTransform =
            glm::translate(identity, local.localPosition) *
            glm::toMat4(local.localRotation) *
            glm::scale(identity, local.localScale);

        world.worldTransform = parentTransform.worldTransform * localTransform;
      });
}

void SceneUpdater::updateCameras(EntityDatabase &entityDatabase) {
  LIQUID_PROFILE_EVENT("SceneUpdater::updateCameras");

  entityDatabase.iterateEntities<PerspectiveLens, WorldTransform, Camera>(
      [](auto entity, const PerspectiveLens &lens, const WorldTransform &world,
         Camera &camera) {
        camera.projectionMatrix = glm::perspective(
            glm::radians(lens.fovY), lens.aspectRatio, lens.near, lens.far);

        camera.viewMatrix = glm::inverse(world.worldTransform);
        camera.projectionViewMatrix =
            camera.projectionMatrix * camera.viewMatrix;
      });
}

void SceneUpdater::updateLights(EntityDatabase &entityDatabase) {
  LIQUID_PROFILE_EVENT("SceneUpdater::updateLights");

  entityDatabase.iterateEntities<WorldTransform, DirectionalLight>(
      [](auto entity, const WorldTransform &world, DirectionalLight &light) {
        glm::quat rotation;
        glm::vec3 empty3;
        glm::vec4 empty4;
        glm::vec3 position;

        glm::decompose(world.worldTransform, empty3, rotation, position, empty3,
                       empty4);

        rotation = glm::conjugate(rotation);

        light.direction = glm::normalize(
            glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
      });
}

} // namespace liquid
