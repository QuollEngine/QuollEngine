#include "liquid/core/Base.h"
#include "SceneUpdater.h"

namespace liquid {

void SceneUpdater::update(EntityContext &entityContext) {
  LIQUID_PROFILE_EVENT("SceneUpdater::update");
  updateTransforms(entityContext);
  updateCameras(entityContext);
  updateLights(entityContext);
}

void SceneUpdater::updateTransforms(EntityContext &entityContext) {
  LIQUID_PROFILE_EVENT("SceneUpdater::updateTransforms");

  entityContext
      .iterateEntities<LocalTransformComponent, WorldTransformComponent>(
          [this, &entityContext](auto entity, LocalTransformComponent &local,
                                 WorldTransformComponent &world) {
            if (entityContext.hasComponent<ParentComponent>(entity))
              return;

            glm::mat4 identity{1.0f};
            glm::mat4 localTransform =
                glm::translate(identity, local.localPosition) *
                glm::toMat4(local.localRotation) *
                glm::scale(identity, local.localScale);

            world.worldTransform = localTransform;
          });

  entityContext.iterateEntities<LocalTransformComponent,
                                WorldTransformComponent, ParentComponent>(
      [this, &entityContext](auto entity, LocalTransformComponent &local,
                             WorldTransformComponent &world,
                             const ParentComponent &parent) {
        auto &parentTransform =
            entityContext.getComponent<WorldTransformComponent>(parent.parent);

        glm::mat4 identity{1.0f};
        glm::mat4 localTransform =
            glm::translate(identity, local.localPosition) *
            glm::toMat4(local.localRotation) *
            glm::scale(identity, local.localScale);

        world.worldTransform = parentTransform.worldTransform * localTransform;
      });
}

void SceneUpdater::updateCameras(EntityContext &entityContext) {
  LIQUID_PROFILE_EVENT("SceneUpdater::updateCameras");

  entityContext.iterateEntities<PerspectiveLensComponent,
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

void SceneUpdater::updateLights(EntityContext &entityContext) {
  LIQUID_PROFILE_EVENT("SceneUpdater::updateLights");

  entityContext
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
