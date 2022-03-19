#include "liquid/core/Base.h"
#include "RenderData.h"

namespace liquid {

RenderData::RenderData(EntityContext &entityContext, Scene *scene,
                       const std::vector<SharedPtr<Material>> &shadowMaterials,
                       rhi::ResourceRegistry &registry)
    : mEntityContext(entityContext), mScene(scene),
      mShadowMaterials(shadowMaterials), mRegistry(registry) {

  mSceneBuffer = mRegistry.setBuffer(
      {rhi::BufferType::Uniform, sizeof(SceneBufferObject)});
}

void RenderData::update() {
  Entity entity = mEnvironmentMapEntity;

  mEntityContext.iterateEntities<EnvironmentComponent>(
      [&entity](Entity e,
                const EnvironmentComponent &environmentComponent) mutable {
        if (entity != e) {
          entity = e;
        }
      });

  if (entity < ENTITY_MAX && entity != mEnvironmentMapEntity) {

    if (mEntityContext.hasEntity(mEnvironmentMapEntity)) {
      mEntityContext.deleteEntity(mEnvironmentMapEntity);
    }
    mEnvironmentChanged = true;
    mEnvironmentMapEntity = entity;
  }

  if (mEntityContext.hasEntity(mEnvironmentMapEntity) &&
      mEntityContext.hasComponent<EnvironmentComponent>(
          mEnvironmentMapEntity)) {
    mSceneData.hasIBL.x = 1;
  }

  size_t i = 0;
  mEntityContext.iterateEntities<LightComponent>(
      [&i, this](Entity entity, const LightComponent &lightComponent) {
        const auto &light = lightComponent.light;
        mSceneData.lights.at(i).color = light->getColor();
        mSceneData.lights.at(i).direction =
            glm::vec4(light->getDirection(), light->getIntensity());
        mSceneData.lights.at(i).type.x = light->getType();
        mSceneData.lights.at(i).lightSpaceMatrix =
            light->getProjectionViewMatrix();
        mShadowMaterials.at(i)->updateProperty(
            "lightMatrix", mSceneData.lights.at(i).lightSpaceMatrix);
        i++;
      });

  mSceneData.numLights.x = static_cast<uint32_t>(i);
  mRegistry.setBuffer(
      {rhi::BufferType::Uniform, sizeof(SceneBufferObject), &mSceneData},
      mSceneBuffer);
}

std::array<rhi::TextureHandle, 3> RenderData::getEnvironmentTextures() const {
  if (mEntityContext.hasEntity(mEnvironmentMapEntity) &&
      mEntityContext.hasComponent<EnvironmentComponent>(
          mEnvironmentMapEntity)) {

    const auto &environment = mEntityContext.getComponent<EnvironmentComponent>(
        mEnvironmentMapEntity);

    return {environment.irradianceMap, environment.specularMap,
            environment.brdfLUT};
  }

  return {};
}

void RenderData::cleanEnvironmentChangeFlag() { mEnvironmentChanged = false; }

} // namespace liquid
