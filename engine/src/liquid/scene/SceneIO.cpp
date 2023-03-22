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

  loadEnvironment(persistentZone);

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
                                            mEntityIdCache, Entity::Null);
  if (res.hasData()) {
    mScene.activeCamera = res.getData();
  } else {
    mScene.activeCamera = mScene.dummyCamera;
  }

  return entities;
}

void SceneIO::saveEntity(Entity entity, const Path &path) {
  detail::EntitySerializer serializer(mAssetRegistry, mScene.entityDatabase);

  if (mScene.entityDatabase.has<Parent>(entity)) {
    auto parent = mScene.entityDatabase.get<Parent>(entity).parent;
    if (!mScene.entityDatabase.has<Id>(parent)) {
      saveEntity(parent, path);
    }
  }

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

Result<bool> SceneIO::saveStartingCamera(const Path &path) {
  if (!mScene.entityDatabase.has<Id>(mScene.activeCamera)) {
    return Result<bool>::Error("Entity does not have an id");
  }

  if (!mScene.entityDatabase.has<PerspectiveLens>(mScene.activeCamera)) {
    return Result<bool>::Error("Entity does not have a camera");
  }

  std::ifstream stream(path);
  auto node = YAML::Load(stream);
  node["zones"][node["persistentZone"].as<uint32_t>()]["startingCamera"] =
      mScene.entityDatabase.get<Id>(mScene.activeCamera).id;
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
      mScene.activeCamera = res.getData();
      saveStartingCamera(path);
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

Result<bool> SceneIO::saveEnvironment(const Path &path) {
  std::ifstream stream(path);
  auto node = YAML::Load(stream);
  stream.close();

  auto zone = node["zones"][node["persistentZone"].as<uint32_t>()];
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
        auto relPath = mAssetRegistry.getEnvironments()
                           .getAsset(skybox.texture)
                           .relativePath.string();
        std::replace(relPath.begin(), relPath.end(), '\\', '/');

        skyboxNode = YAML::Node(YAML::NodeType::Map);
        skyboxNode["type"] = "texture";
        skyboxNode["texture"] = relPath;
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
      auto relPath = skybox["texture"].as<String>();
      auto handle = mAssetRegistry.getEnvironments().findHandleByRelativePath(
          Path(relPath));

      EnvironmentSkybox component{EnvironmentSkyboxType::Texture};
      component.texture = handle;

      if (handle != EnvironmentAssetHandle::Invalid) {
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
