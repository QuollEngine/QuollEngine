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

  entityDatabase
      .iterateEntities<LocalTransformComponent, WorldTransformComponent>(
          [this, &entityDatabase](auto entity, LocalTransformComponent &local,
                                  WorldTransformComponent &world) {
            if (entityDatabase.hasComponent<ParentComponent>(entity))
              return;

            glm::mat4 identity{1.0f};
            glm::mat4 localTransform =
                glm::translate(identity, local.localPosition) *
                glm::toMat4(local.localRotation) *
                glm::scale(identity, local.localScale);

            world.worldTransform = localTransform;
          });

  entityDatabase.iterateEntities<LocalTransformComponent,
                                 WorldTransformComponent, ParentComponent>(
      [this, &entityDatabase](auto entity, LocalTransformComponent &local,
                              WorldTransformComponent &world,
                              const ParentComponent &parent) {
        auto &parentTransform =
            entityDatabase.getComponent<WorldTransformComponent>(parent.parent);

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

  entityDatabase.iterateEntities<PerspectiveLensComponent,
                                 WorldTransformComponent, CameraComponent>(
      [](auto entity, const PerspectiveLensComponent &lens,
         const WorldTransformComponent &world, CameraComponent &camera) {
        camera.projectionMatrix =
            glm::perspective(lens.fovY, lens.aspectRatio, lens.near, lens.far);

        camera.projectionMatrix[1][1] *= -1.0f;

        camera.viewMatrix = glm::inverse(world.worldTransform);
        camera.projectionViewMatrix =
            camera.projectionMatrix * camera.viewMatrix;
      });
}

void SceneUpdater::updateLights(EntityDatabase &entityDatabase) {
  LIQUID_PROFILE_EVENT("SceneUpdater::updateLights");

  entityDatabase
      .iterateEntities<WorldTransformComponent, DirectionalLightComponent>(
          [](auto entity, const WorldTransformComponent &world,
             DirectionalLightComponent &light) {
            glm::quat rotation;
            glm::vec3 empty3;
            glm::vec4 empty4;
            glm::vec3 position;

            glm::decompose(world.worldTransform, empty3, rotation, position,
                           empty3, empty4);

            rotation = glm::conjugate(rotation);

            light.direction = glm::normalize(
                glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
          });
}

} // namespace liquid
