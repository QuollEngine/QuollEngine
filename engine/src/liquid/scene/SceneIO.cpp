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

  auto currentZone = scene["zones"][0];

  loadEnvironment(currentZone);

  stream.close();

  std::vector<Entity> entities;
  std::vector<YAML::Node> yamlNodes;
  for (const auto &node : scene["entities"]) {
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

void SceneIO::saveEntities(const std::vector<Entity> &entities,
                           const Path &path) {
  std::ifstream stream(path);
  auto node = YAML::Load(stream);
  stream.close();

  std::unordered_map<Entity, bool> updateCache;

  for (auto entity : entities) {
    updateSceneYaml(entity, node, updateCache);
  }

  std::ofstream wstream(path);
  wstream << node;
  wstream.close();
}

void SceneIO::updateSceneYaml(Entity entity, YAML::Node &node,
                              std::unordered_map<Entity, bool> &updateCache) {
  if (updateCache.contains(entity)) {
    return;
  }

  detail::EntitySerializer serializer(mAssetRegistry, mScene.entityDatabase);

  if (mScene.entityDatabase.has<Parent>(entity)) {
    auto parent = mScene.entityDatabase.get<Parent>(entity).parent;
    if (!mScene.entityDatabase.has<Id>(parent)) {
      updateSceneYaml(parent, node, updateCache);
    }
  }

  if (!mScene.entityDatabase.has<Id>(entity)) {
    mScene.entityDatabase.set<Id>(entity, {generateId()});
  }

  auto id = mScene.entityDatabase.get<Id>(entity).id;

  auto updatedNode = serializer.serialize(entity);
  if (updatedNode.hasData()) {
    if (mEntityIdCache.find(id) == mEntityIdCache.end()) {
      node["entities"].push_back(updatedNode.getData());
    } else {
      size_t i = 0;
      for (; i < node["entities"].size() &&
             node["entities"][i]["id"].as<uint64_t>(0) != id;
           ++i) {
      }

      if (i < node["entities"].size()) {
        node["entities"][i] = updatedNode.getData();
      }
    }
  }
  updateCache.insert_or_assign(entity, true);
  mEntityIdCache.insert({id, entity});
}

void SceneIO::deleteEntities(const std::vector<Entity> &entities,
                             const Path &path) {
  Entity currentActiveCamera = Entity::Null;
  for (auto entity : entities) {
    if (mScene.activeCamera == entity) {
      currentActiveCamera = entity;
    }
  }

  if (currentActiveCamera != Entity::Null) {
    mScene.activeCamera = mScene.dummyCamera;
  }

  std::ifstream stream(path);
  auto node = YAML::Load(stream);
  stream.close();

  std::unordered_map<Entity, bool> deleteCache;

  for (auto entity : entities) {
    removeEntityFromSceneYaml(entity, node, deleteCache);
  }

  if (currentActiveCamera != Entity::Null) {
    detail::SceneLoader sceneLoader(mAssetRegistry, mScene.entityDatabase);
    for (auto [e, _] : mScene.entityDatabase.view<PerspectiveLens>()) {
      if (e != currentActiveCamera && e != mScene.dummyCamera) {
        mScene.activeCamera = e;
        break;
      }
    }
  }

  std::ofstream wstream(path);
  wstream << node;
  wstream.close();
}

void SceneIO::removeEntityFromSceneYaml(
    Entity entity, YAML::Node &node,
    std::unordered_map<Entity, bool> &deleteCache) {
  if (deleteCache.contains(entity)) {
    return;
  }

  if (mScene.entityDatabase.has<Children>(entity)) {
    const auto &children = mScene.entityDatabase.get<Children>(entity);
    for (auto entity : children.children) {
      removeEntityFromSceneYaml(entity, node, deleteCache);
    }
  }

  if (!mScene.entityDatabase.has<Id>(entity)) {
    return;
  }

  auto id = mScene.entityDatabase.get<Id>(entity).id;

  size_t i = 0;
  for (; i < node["entities"].size() &&
         node["entities"][i]["id"].as<uint64_t>(0) != id;
       ++i) {
  }

  node["entities"].remove(i);
  mEntityIdCache.erase(id);
  deleteCache.insert_or_assign(entity, true);
}

Result<bool> SceneIO::saveStartingCamera(const Path &path) {
  if (!mScene.entityDatabase.has<Id>(mScene.activeCamera)) {
    return Result<bool>::Error("Entity does not have an id");
  }

  if (!mScene.entityDatabase.has<PerspectiveLens>(mScene.activeCamera)) {
    return Result<bool>::Error("Entity does not have a camera");
  }

  std::ifstream stream(path);
  auto node = YAML::Load(stream);
  node["zones"][0]["startingCamera"] =
      mScene.entityDatabase.get<Id>(mScene.activeCamera).id;
  stream.close();

  std::ofstream writeStream(path);
  writeStream << node;
  writeStream.close();

  return Result<bool>::Ok(true);
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

Result<bool> SceneIO::saveEnvironment(const Path &path) {
  std::ifstream stream(path);
  auto node = YAML::Load(stream);
  stream.close();

  auto zone = node["zones"][0];
  zone["environment"] = YAML::Null;

  if (mScene.entityDatabase.exists(mScene.environment)) {
    YAML::Node skyboxNode(YAML::NodeType::Null);
    YAML::Node lightingNode(YAML::NodeType::Null);

    if (mScene.entityDatabase.has<EnvironmentSkybox>(mScene.environment)) {
      auto &skybox =
          mScene.entityDatabase.get<EnvironmentSkybox>(mScene.environment);

      if (skybox.type == EnvironmentSkyboxType::Color) {
        skyboxNode = YAML::Node(YAML::NodeType::Map);
        skyboxNode["type"] = "color";
        skyboxNode["color"] = skybox.color;
      } else if (skybox.type == EnvironmentSkyboxType::Texture &&
                 mAssetRegistry.getEnvironments().hasAsset(skybox.texture)) {
        auto uuid =
            mAssetRegistry.getEnvironments().getAsset(skybox.texture).uuid;

        skyboxNode = YAML::Node(YAML::NodeType::Map);
        skyboxNode["type"] = "texture";
        skyboxNode["texture"] = uuid;
      }
    }

    if (mScene.entityDatabase.has<EnvironmentLightingSkyboxSource>(
            mScene.environment)) {
      lightingNode = YAML::Node(YAML::NodeType::Map);
      lightingNode["source"] = "skybox";
    }

    if (skyboxNode.IsMap() || lightingNode.IsMap()) {
      zone["environment"] = YAML::Node(YAML::NodeType::Map);
      zone["environment"]["skybox"] = skyboxNode;
      zone["environment"]["lighting"] = lightingNode;
    }
  }

  stream.close();

  YAML::Emitter emitter;
  emitter.SetNullFormat(YAML::LowerNull);
  emitter << node;

  std::ofstream writeStream(path);
  writeStream << emitter.c_str();
  writeStream.close();

  return Result<bool>::Ok(true);
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
