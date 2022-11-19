#include "liquid/core/Base.h"
#include "SceneIO.h"

#include "private/SceneLoader.h"
#include "private/EntitySerializer.h"

namespace liquid {

SceneIO::SceneIO(AssetRegistry &assetRegistry, Scene &scene)
    : mScene(scene), mAssetRegistry(assetRegistry) {
  reset();
}

std::vector<Entity> SceneIO::loadScene(const Path &path) {
  detail::SceneLoader sceneLoader(mAssetRegistry, mScene.entityDatabase);

  if (!std::filesystem::is_regular_file(path)) {
    return {};
  }

  std::ifstream stream(path, std::ios::in);
  auto scene = YAML::Load(stream);

  auto persistentZoneIndex = scene["persistentZone"].as<uint32_t>();
  auto persistentZone = scene["zones"][persistentZoneIndex];

  stream.close();
  auto entitiesPath =
      path.parent_path() / persistentZone["entities"].as<String>();

  std::vector<Entity> entities;
  std::vector<YAML::Node> yamlNodes;

  if (std::filesystem::is_directory(entitiesPath)) {
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator(entitiesPath)) {
      std::ifstream stream(entry.path(), std::ios::in);

      auto node = YAML::Load(stream);
      auto res = createEntityFromNode(node);

      if (res.hasData()) {
        entities.push_back(res.getData());
        yamlNodes.push_back(std::move(node));
      }

      stream.close();
    }
  }

  for (size_t i = 0; i < entities.size(); ++i) {
    auto &node = yamlNodes.at(i);

    if (!node["components"]) {
      node["components"] = YAML::Node(YAML::NodeType::Map);
    }

    sceneLoader.loadComponents(node, entities.at(i), mEntityIdCache);
  }

  auto res = sceneLoader.loadStartingCamera(persistentZone["startingCamera"],
                                            mEntityIdCache, EntityNull);
  if (res.hasData()) {
    mScene.activeCamera = res.getData();
  } else {
    mScene.activeCamera = mScene.dummyCamera;
  }

  return entities;
}

void SceneIO::saveEntity(Entity entity, const Path &path) {
  detail::EntitySerializer serializer(mAssetRegistry, mScene.entityDatabase);

  if (!mScene.entityDatabase.has<Id>(entity)) {
    mScene.entityDatabase.set<Id>(entity, {generateId()});
  }

  auto id = mScene.entityDatabase.get<Id>(entity).id;
  mEntityIdCache.insert({id, entity});

  auto node = serializer.serialize(entity);

  if (node.hasData() && std::filesystem::is_regular_file(path)) {
    std::ofstream stream(getEntityPath(entity, path), std::ios::out);
    stream << node.getData();
    stream.close();
  }
}

Result<bool> SceneIO::saveStartingCamera(Entity entity, const Path &path) {
  if (!mScene.entityDatabase.has<Id>(entity)) {
    return Result<bool>::Error("Entity does not have an id");
  }

  if (!mScene.entityDatabase.has<PerspectiveLens>(entity)) {
    return Result<bool>::Error("Entity does not have a camera");
  }

  std::ifstream stream(path);
  auto node = YAML::Load(stream);
  node["zones"][node["persistentZone"].as<uint32_t>()]["startingCamera"] =
      mScene.entityDatabase.get<Id>(entity).id;
  stream.close();

  std::ofstream writeStream(path);
  writeStream << node;
  writeStream.close();

  return Result<bool>::Ok(true);
}

void SceneIO::deleteEntityFilesAndRelations(Entity entity, const Path &path) {
  if (mScene.entityDatabase.has<PerspectiveLens>(entity)) {
    detail::SceneLoader sceneLoader(mAssetRegistry, mScene.entityDatabase);
    auto res =
        sceneLoader.loadStartingCamera(YAML::Node{}, mEntityIdCache, entity);
    if (res.hasData()) {
      saveStartingCamera(res.getData(), path);
      mScene.activeCamera = res.getData();
    } else {
      mScene.activeCamera = mScene.dummyCamera;
    }
  }

  if (mScene.entityDatabase.has<Children>(entity)) {
    const auto &children = mScene.entityDatabase.get<Children>(entity);
    for (auto entity : children.children) {
      deleteEntityFilesAndRelations(entity, path);
    }
  }

  if (mScene.entityDatabase.has<Id>(entity)) {
    auto id = mScene.entityDatabase.get<Id>(entity).id;
    mEntityIdCache.insert({id, entity});

    std::filesystem::remove(getEntityPath(entity, path));
  }
}

void SceneIO::reset() {
  mScene.entityDatabase.destroy();
  mEntityIdCache.clear();
  auto dummyCamera = mScene.entityDatabase.create();
  mScene.entityDatabase.set<Camera>(dummyCamera, {});
  mScene.dummyCamera = dummyCamera;
  mScene.activeCamera = dummyCamera;
}

Path SceneIO::getEntityPath(Entity entity, const Path &path) {
  std::ifstream stream(path, std::ios::in);
  auto scene = YAML::Load(stream);

  auto persistentZoneIndex = scene["persistentZone"].as<uint32_t>();
  auto persistentZone = scene["zones"][persistentZoneIndex];

  stream.close();
  auto entitiesPath =
      path.parent_path() / persistentZone["entities"].as<String>();

  auto id = mScene.entityDatabase.get<Id>(entity).id;
  return entitiesPath / (std::to_string(id) + ".lqnode");
}

uint64_t SceneIO::generateId() { return mLastId++; }

Result<Entity> SceneIO::createEntityFromNode(const YAML::Node &node) {
  if (node["id"] && node["id"].IsScalar()) {
    auto id = node["id"].as<uint64_t>(0);

    if (id > 0 && mEntityIdCache.find(id) == mEntityIdCache.end()) {
      auto entity = mScene.entityDatabase.create();
      mScene.entityDatabase.set<Id>(entity, {id});

      if (mLastId <= id) {
        mLastId = id + 1;
      }
      mEntityIdCache.insert({id, entity});
      return Result<Entity>::Ok(entity);
    }
  }

  return Result<Entity>::Error("Node does not have an ID");
}

} // namespace liquid
