#include "core/Base.h"
#include "VulkanRenderData.h"

namespace liquid {

constexpr size_t MAX_LIGHTS = 16;

struct LightData {
  // NOLINTNEXTLINE
  alignas(16) glm::vec4 color;
  // NOLINTNEXTLINE
  alignas(16) glm::vec4 direction;
  // NOLINTNEXTLINE
  alignas(16) glm::uvec4 type;
  // NOLINTNEXTLINE
  alignas(16) glm::mat4 lightSpaceMatrix;
};

struct SceneBufferObject {
  std::array<LightData, MAX_LIGHTS> lights{};
  glm::uvec4 numLights;
  glm::uvec4 hasIBL;
};

VulkanRenderData::VulkanRenderData(
    EntityContext &entityContext_, Scene *scene_,
    ResourceAllocator *resourceAllocator, const SharedPtr<Texture> &shadowmaps_,
    const std::vector<SharedPtr<Material>> &shadowMaterials_)
    : entityContext(entityContext_), scene(scene_),
      shadowMaterials(shadowMaterials_), shadowmaps(shadowmaps_) {

  sceneBuffer =
      resourceAllocator->createUniformBuffer(sizeof(SceneBufferObject));
}

void VulkanRenderData::update() {
  SceneBufferObject sceneData{};
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
      [&i, &sceneData, this](Entity entity,
                             const LightComponent &lightComponent) {
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
  sceneBuffer->update(&sceneData);
}

std::array<SharedPtr<Texture>, 3>
VulkanRenderData::getEnvironmentTextures() const {
  if (entityContext.hasEntity(environmentMapEntity) &&
      entityContext.hasComponent<EnvironmentComponent>(environmentMapEntity)) {

    const auto &environment =
        entityContext.getComponent<EnvironmentComponent>(environmentMapEntity);

    return {environment.irradianceMap, environment.specularMap,
            environment.brdfLUT};
  }

  return {};
}

void VulkanRenderData::cleanEnvironmentChangeFlag() {
  environmentChanged = false;
}

} // namespace liquid
