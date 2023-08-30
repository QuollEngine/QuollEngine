#include "quoll/core/Base.h"
#include "SceneIO.h"

#include "private/SceneLoader.h"
#include "private/EntitySerializer.h"

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

  for (size_t i = 0; i < entities.size(); ++i) {
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

} // namespace quoll
