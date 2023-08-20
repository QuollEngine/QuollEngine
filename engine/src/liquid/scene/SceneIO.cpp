#include "liquid/core/Base.h"
#include "SceneIO.h"

#include "private/SceneLoader.h"
#include "private/EntitySerializer.h"

namespace liquid {

SceneIO::SceneIO(AssetRegistry &assetRegistry, Scene &scene)
    : mScene(scene), mAssetRegistry(assetRegistry) {
  reset();
}

std::vector<Entity> SceneIO::loadScene(SceneAssetHandle scene) {
  detail::SceneLoader sceneLoader(mAssetRegistry, mScene.entityDatabase);

  const auto &root = mAssetRegistry.getScenes().getAsset(scene).data.data;

  auto currentZone = root["zones"][0];

  loadEnvironment(currentZone);

  std::vector<Entity> entities;
  std::vector<YAML::Node> yamlNodes;
  for (const auto &node : root["entities"]) {
    auto res = createEntityFromNode(node);

    if (res.hasData()) {
      entities.push_back(res.getData());
      yamlNodes.push_back(node);
    }
  }

  for (size_t i = 0; i < entities.size(); ++i) {
    auto &node = yamlNodes.at(i);

    sceneLoader.loadComponents(node, entities.at(i), mEntityIdCache);
  }

  auto res = sceneLoader.loadStartingCamera(currentZone["startingCamera"],
                                            mEntityIdCache);
  if (res.hasData()) {
    mScene.activeCamera = res.getData();
  } else {
    mScene.activeCamera = mScene.dummyCamera;
  }

  return entities;
}

void SceneIO::reset() {
  mScene.entityDatabase.destroy();
  mEntityIdCache.clear();
  auto dummyCamera = mScene.entityDatabase.create();
  mScene.entityDatabase.set<Camera>(dummyCamera, {});
  mScene.entityDatabase.set<PerspectiveLens>(dummyCamera, {});

  mScene.dummyCamera = dummyCamera;
  mScene.activeCamera = dummyCamera;
}

void SceneIO::loadEnvironment(const YAML::Node &zone) {
  if (!mScene.entityDatabase.exists(mScene.environment)) {
    mScene.environment = mScene.entityDatabase.create();
  }

  if (!zone["environment"] || zone["environment"].IsNull() ||
      !zone["environment"].IsMap()) {
    return;
  }

  auto skybox = zone["environment"]["skybox"];
  if (skybox && skybox.IsMap() && skybox["type"] && skybox["type"].IsScalar()) {
    auto skyboxType = skybox["type"].as<String>();

    if (skyboxType == "color") {
      EnvironmentSkybox component{EnvironmentSkyboxType::Color};
      component.color = skybox["color"].as<glm::vec4>(glm::vec4{0.0f});
      mScene.entityDatabase.set(mScene.environment, component);
    } else if (skyboxType == "texture" && skybox["texture"].IsScalar()) {
      auto uuid = skybox["texture"].as<String>();
      auto handle = mAssetRegistry.getEnvironments().findHandleByUuid(uuid);

      EnvironmentSkybox component{EnvironmentSkyboxType::Texture};
      component.texture = handle;

      if (handle != EnvironmentAssetHandle::Null) {
        mScene.entityDatabase.set(mScene.environment, component);
      }
    }
  }

  auto lighting = zone["environment"]["lighting"];
  if (lighting && lighting.IsMap() && lighting["source"] &&
      lighting["source"].IsScalar()) {
    auto sourceType = lighting["source"].as<String>();
    if (sourceType == "skybox") {
      mScene.entityDatabase.set<EnvironmentLightingSkyboxSource>(
          mScene.environment, {});
    }
  }
}

Result<Entity> SceneIO::createEntityFromNode(const YAML::Node &node) {
  if (node["id"] && node["id"].IsScalar()) {
    auto id = node["id"].as<uint64_t>(0);

    if (id > 0 && mEntityIdCache.find(id) == mEntityIdCache.end()) {
      auto entity = mScene.entityDatabase.create();
      mScene.entityDatabase.set<Id>(entity, {id});

      mEntityIdCache.insert({id, entity});
      return Result<Entity>::Ok(entity);
    }
  }

  return Result<Entity>::Error("Node does not have an ID");
}

} // namespace liquid
