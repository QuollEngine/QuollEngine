#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/Scene.h"
#include "SceneIO.h"
#include "SceneLoader.h"

namespace quoll {

SceneIO::SceneIO(AssetCache &assetCache, Scene &scene)
    : mScene(scene), mAssetCache(assetCache) {
  reset();
}

std::vector<Entity> SceneIO::loadScene(AssetHandle<SceneAsset> scene) {
  detail::SceneLoader sceneLoader(mAssetCache, mScene.entityDatabase);

  const auto &root = mAssetCache.getRegistry().get(scene).data;

  auto currentZone = root["zones"][0];

  std::vector<Entity> entities;
  std::vector<YAML::Node> yamlNodes;
  for (const auto &node : root["entities"]) {
    auto res = createEntityFromNode(node);

    if (res) {
      entities.push_back(res);
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

    if (res) {
      mScene.activeCamera = res;
    } else {
      mScene.activeCamera = mScene.dummyCamera;
    }
  }

  {
    auto res =
        sceneLoader.loadEnvironment(currentZone["environment"], mEntityIdCache);

    if (res) {
      mScene.activeEnvironment = res;
    } else {
      mScene.activeEnvironment = mScene.dummyEnvironment;
    }
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

  mScene.dummyEnvironment = mScene.entityDatabase.create();
}

Result<Entity> SceneIO::createEntityFromNode(const YAML::Node &node) {
  if (node["id"] && node["id"].IsScalar()) {
    auto id = node["id"].as<u64>(0);

    if (id > 0 && mEntityIdCache.find(id) == mEntityIdCache.end()) {
      auto entity = mScene.entityDatabase.create();
      mScene.entityDatabase.set<Id>(entity, {id});

      mEntityIdCache.insert({id, entity});
      return entity;
    }
  }

  return Error("Node does not have an ID");
}

} // namespace quoll
