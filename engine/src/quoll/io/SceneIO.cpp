#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/Scene.h"
#include "SceneIO.h"
#include "SceneLoader.h"

namespace quoll {

SceneIO::SceneIO(AssetRegistry &assetRegistry, Scene &scene)
    : mScene(scene), mAssetRegistry(assetRegistry) {
  reset();
}

std::vector<Entity> SceneIO::loadScene(SceneAssetHandle scene) {
  detail::SceneLoader sceneLoader(mAssetRegistry, mScene.entityDatabase);

  const auto &root = mAssetRegistry.getScenes().getAsset(scene).data.data;

  auto currentZone = root["zones"][0];

  std::vector<Entity> entities;
  std::vector<YAML::Node> yamlNodes;
  for (const auto &node : root["entities"]) {
    auto res = createEntityFromNode(node);

    if (res.hasData()) {
      entities.push_back(res.getData());
      yamlNodes.push_back(node);
    }
  }

  for (usize i = 0; i < entities.size(); ++i) {
    auto &node = yamlNodes.at(i);

    sceneLoader.loadComponents(node, entities.at(i), mEntityIdCache);
  }

  {
    auto res = sceneLoader.loadStartingCamera(currentZone["startingCamera"],
                                              mEntityIdCache);

    if (res.hasData()) {
      mScene.activeCamera = res.getData();
    } else {
      mScene.activeCamera = mScene.dummyCamera;
    }
  }

  {
    auto res =
        sceneLoader.loadEnvironment(currentZone["environment"], mEntityIdCache);

    if (res.hasData()) {
      mScene.activeEnvironment = res.getData();
    } else {
      mScene.activeEnvironment = mScene.dummyEnvironment;
    }
  }

  return entities;
}

void SceneIO::reset() {
  mEntityIdCache.clear();
  mScene.dummyCamera = mScene.entityDatabase.entity();
  mScene.dummyCamera.set<Camera>({});
  mScene.dummyCamera.set<PerspectiveLens>({});
  mScene.activeCamera = mScene.dummyCamera;
  mScene.dummyEnvironment = mScene.entityDatabase.entity();
}

Result<Entity> SceneIO::createEntityFromNode(const YAML::Node &node) {
  if (node["id"] && node["id"].IsScalar()) {
    auto id = node["id"].as<u64>(0);

    if (id > 0 && mEntityIdCache.find(id) == mEntityIdCache.end()) {
      auto entity = mScene.entityDatabase.entity();
      entity.set<Id>({id});

      mEntityIdCache.insert({id, entity});
      return Result<Entity>::Ok(entity);
    }
  }

  return Result<Entity>::Error("Node does not have an ID");
}

} // namespace quoll
