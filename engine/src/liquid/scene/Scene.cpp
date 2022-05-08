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

  mEntityContext.iterateEntities<TransformComponent>(
      [this](auto entity, TransformComponent &transform) {
        if (mEntityContext.hasComponent<ParentComponent>(entity))
          return;

        glm::mat4 identity{1.0f};
        glm::mat4 localTransform =
            glm::translate(identity, transform.localPosition) *
            glm::toMat4(transform.localRotation) *
            glm::scale(identity, transform.localScale);

        transform.worldTransform = localTransform;
      });

  mEntityContext.iterateEntities<TransformComponent, ParentComponent>(
      [this](auto entity, TransformComponent &transform,
             const ParentComponent &parent) {
        auto &parentTransform =
            mEntityContext.getComponent<TransformComponent>(parent.parent);

        glm::mat4 identity{1.0f};
        glm::mat4 localTransform =
            glm::translate(identity, transform.localPosition) *
            glm::toMat4(transform.localRotation) *
            glm::scale(identity, transform.localScale);

        transform.worldTransform =
            parentTransform.worldTransform * localTransform;
      });
}

void Scene::updateCameras() {
  LIQUID_PROFILE_EVENT("Scene::updateCameras");

  mEntityContext.iterateEntities<PerspectiveLensComponent, TransformComponent,
                                 CameraComponent>(
      [](auto entity, const PerspectiveLensComponent &lens,
         const TransformComponent &transform, CameraComponent &camera) {
        camera.projectionMatrix =
            glm::perspective(lens.fovY, lens.aspectRatio, lens.near, lens.far);

        camera.projectionMatrix[1][1] *= -1.0f;

        camera.viewMatrix = glm::inverse(transform.worldTransform);
        camera.projectionViewMatrix =
            camera.projectionMatrix * camera.viewMatrix;
      });
}

void Scene::updateLights() {
  LIQUID_PROFILE_EVENT("Scene::updateLights");

  mEntityContext.iterateEntities<TransformComponent, DirectionalLightComponent>(
      [](auto entity, const TransformComponent &transform,
         DirectionalLightComponent &light) {
        glm::quat rotation;
        glm::vec3 empty3;
        glm::vec4 empty4;
        glm::vec3 position;

        glm::decompose(transform.worldTransform, empty3, rotation, position,
                       empty3, empty4);

        rotation = glm::conjugate(rotation);

        light.direction = glm::normalize(
            glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)));
      });
}

} // namespace liquid
