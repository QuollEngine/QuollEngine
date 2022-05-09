#include "liquid/core/Base.h"
#include "Scene.h"

namespace liquid {

Scene::Scene(EntityContext &entityContext) : mEntityContext(entityContext) {}

Scene::~Scene() {}

void Scene::update() {
  LIQUID_PROFILE_EVENT("Scene::update");
  updateTransforms();
  updateCameras();
  updateLights();
}

void Scene::updateTransforms() {
  LIQUID_PROFILE_EVENT("Scene::updateTransforms");

  mEntityContext
      .iterateEntities<LocalTransformComponent, WorldTransformComponent>(
          [this](auto entity, LocalTransformComponent &local,
                 WorldTransformComponent &world) {
            if (mEntityContext.hasComponent<ParentComponent>(entity))
              return;

            glm::mat4 identity{1.0f};
            glm::mat4 localTransform =
                glm::translate(identity, local.localPosition) *
                glm::toMat4(local.localRotation) *
                glm::scale(identity, local.localScale);

            world.worldTransform = localTransform;
          });

  mEntityContext.iterateEntities<LocalTransformComponent,
                                 WorldTransformComponent, ParentComponent>(
      [this](auto entity, LocalTransformComponent &local,
             WorldTransformComponent &world, const ParentComponent &parent) {
        auto &parentTransform =
            mEntityContext.getComponent<WorldTransformComponent>(parent.parent);

        glm::mat4 identity{1.0f};
        glm::mat4 localTransform =
            glm::translate(identity, local.localPosition) *
            glm::toMat4(local.localRotation) *
            glm::scale(identity, local.localScale);

        world.worldTransform = parentTransform.worldTransform * localTransform;
      });
}

void Scene::updateCameras() {
  LIQUID_PROFILE_EVENT("Scene::updateCameras");

  mEntityContext.iterateEntities<PerspectiveLensComponent,
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

void Scene::updateLights() {
  LIQUID_PROFILE_EVENT("Scene::updateLights");

  mEntityContext
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
