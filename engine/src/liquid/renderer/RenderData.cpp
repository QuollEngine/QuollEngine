#include "liquid/core/Base.h"
#include "RenderData.h"

namespace liquid {

RenderData::RenderData(EntityContext &entityContext_, Scene *scene_,
                       const std::vector<SharedPtr<Material>> &shadowMaterials_,
                       experimental::ResourceRegistry &registry_)
    : entityContext(entityContext_), scene(scene_),
      shadowMaterials(shadowMaterials_), registry(registry_) {

  sceneBuffer =
      registry.addBuffer({BufferType::Uniform, sizeof(SceneBufferObject)});
}

void RenderData::update() {
  Entity entity = environmentMapEntity;

  entityContext.iterateEntities<EnvironmentComponent>(
      [&entity](Entity e,
                const EnvironmentComponent &environmentComponent) mutable {
        if (entity != e) {
          entity = e;
        }
      });

  if (entity < ENTITY_MAX && entity != environmentMapEntity) {

    if (entityContext.hasEntity(environmentMapEntity)) {
      entityContext.deleteEntity(environmentMapEntity);
    }
    environmentChanged = true;
    environmentMapEntity = entity;
  }

  if (entityContext.hasEntity(environmentMapEntity) &&
      entityContext.hasComponent<EnvironmentComponent>(environmentMapEntity)) {
    sceneData.hasIBL.x = 1;
  }

  size_t i = 0;
  entityContext.iterateEntities<LightComponent>(
      [&i, this](Entity entity, const LightComponent &lightComponent) {
        const auto &light = lightComponent.light;
        sceneData.lights.at(i).color = light->getColor();
        sceneData.lights.at(i).direction =
            glm::vec4(light->getDirection(), light->getIntensity());
        sceneData.lights.at(i).type.x = light->getType();
        sceneData.lights.at(i).lightSpaceMatrix =
            light->getProjectionViewMatrix();
        shadowMaterials.at(i)->updateProperty(
            "lightMatrix", sceneData.lights.at(i).lightSpaceMatrix);
        i++;
      });

  sceneData.numLights.x = static_cast<uint32_t>(i);
  registry.updateBuffer(sceneBuffer, {BufferType::Uniform,
                                      sizeof(SceneBufferObject), &sceneData});
}

std::array<TextureHandle, 3> RenderData::getEnvironmentTextures() const {
  if (entityContext.hasEntity(environmentMapEntity) &&
      entityContext.hasComponent<EnvironmentComponent>(environmentMapEntity)) {

    const auto &environment =
        entityContext.getComponent<EnvironmentComponent>(environmentMapEntity);

    return {environment.irradianceMap, environment.specularMap,
            environment.brdfLUT};
  }

  return {};
}

void RenderData::cleanEnvironmentChangeFlag() { environmentChanged = false; }

} // namespace liquid
