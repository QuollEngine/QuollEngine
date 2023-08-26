#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"
#include "liquid/scene/private/EntitySerializer.h"

#include "SceneWriter.h"

namespace liquid::editor {

SceneWriter::SceneWriter(Scene &scene, AssetRegistry &assetRegistry)
    : mScene(scene), mAssetRegistry(assetRegistry) {}

void SceneWriter::open(Path sourcePath) {
  mSourcePath = sourcePath;

  mEntityIdCache.clear();

  if (mStream.is_open()) {
    mStream.close();
  }

  mStream.open(mSourcePath, std::ios::in);
  mRoot = YAML::Load(mStream);
  mStream.close();

  for (auto [entity, id] : mScene.entityDatabase.view<Id>()) {
    mEntityIdCache.insert_or_assign(id.id, entity);
    mLastId = std::max(id.id, mLastId);
  }
  mLastId++;
}

void SceneWriter::saveEntities(const std::vector<Entity> &entities) {
  std::unordered_map<Entity, bool> updateCache;

  for (auto entity : entities) {
    updateSceneYaml(entity, mRoot, updateCache);
  }

  save();
}

void SceneWriter::updateSceneYaml(
    Entity entity, YAML::Node &node,
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
  mEntityIdCache.insert_or_assign(id, entity);
}

void SceneWriter::deleteEntities(const std::vector<Entity> &entities) {
  std::unordered_map<Entity, bool> deleteCache;

  for (auto entity : entities) {
    removeEntityFromSceneYaml(entity, mRoot, deleteCache);
  }

  save();
}

void SceneWriter::saveScene() {
  updateStartingCamera();
  updateEnvironment();
  save();
}

void SceneWriter::removeEntityFromSceneYaml(
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

  if (mRoot["zones"][0]["startingCamera"].as<uint64_t>(0) == id) {
    mRoot["zones"][0]["startingCamera"] = YAML::Node(YAML::NodeType::Null);
  }

  if (mRoot["zones"][0]["environment"].as<uint64_t>(0) == id) {
    mRoot["zones"][0]["environment"] = YAML::Node(YAML::NodeType::Null);
  }

  mEntityIdCache.erase(id);
  deleteCache.insert_or_assign(entity, true);
}

void SceneWriter::updateStartingCamera() {
  if (!mScene.entityDatabase.has<Id>(mScene.activeCamera) ||
      !mScene.entityDatabase.has<PerspectiveLens>(mScene.activeCamera)) {
    return;
  }

  mRoot["zones"][0]["startingCamera"] =
      mScene.entityDatabase.get<Id>(mScene.activeCamera).id;
}

void SceneWriter::updateEnvironment() {
  if (!mScene.entityDatabase.has<Id>(mScene.activeEnvironment)) {
    return;
  }

  mRoot["zones"][0]["environment"] =
      mScene.entityDatabase.get<Id>(mScene.activeEnvironment).id;
}

void SceneWriter::save() {
  YAML::Emitter emitter;
  emitter.SetNullFormat(YAML::LowerNull);
  emitter.SetSeqFormat(YAML::Block);
  emitter.SetMapFormat(YAML::Block);
  emitter << mRoot;

  mStream.open(mSourcePath, std::ios::trunc | std::ios::out);
  mStream << emitter.c_str();
  mStream.close();
}

uint64_t SceneWriter::generateId() { return mLastId++; }

} // namespace liquid::editor
