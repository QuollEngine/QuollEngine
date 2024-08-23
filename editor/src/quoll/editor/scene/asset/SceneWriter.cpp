#include "quoll/core/Base.h"
#include "quoll/core/Id.h"
#include "quoll/io/EntitySerializer.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/yaml/Yaml.h"
#include "SceneWriter.h"

namespace quoll::editor {

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

  auto query = mScene.entityDatabase.query<Id>();

  query.each([this](flecs::entity entity, auto &id) {
    mEntityIdCache.insert_or_assign(id.id, entity);
    mLastId = std::max(id.id, mLastId);
  });

  mLastId++;
}

void SceneWriter::syncEntities(const std::vector<Entity> &entities) {
  std::unordered_map<flecs::id_t, bool> updateCache;

  for (auto entity : entities) {
    updateSceneYaml(entity, mRoot, updateCache);
  }

  save();
}

void SceneWriter::updateSceneYaml(
    Entity entity, YAML::Node &node,
    std::unordered_map<flecs::id_t, bool> &updateCache) {
  if (updateCache.contains(entity)) {
    return;
  }

  detail::EntitySerializer serializer(mAssetRegistry, mScene.entityDatabase);

  if (entity.has<Parent>()) {
    auto parent = entity.get_ref<Parent>()->parent;
    if (!parent.has<Id>()) {
      updateSceneYaml(parent, node, updateCache);
    }
  }

  if (!entity.has<Id>()) {
    entity.set(Id{generateId()});
  }

  auto id = entity.get_ref<Id>()->id;

  auto updatedNode = serializer.serialize(entity);
  if (updatedNode.hasData()) {
    if (mEntityIdCache.find(id) == mEntityIdCache.end()) {
      node["entities"].push_back(updatedNode.getData());
    } else {
      usize i = 0;
      for (; i < node["entities"].size() &&
             node["entities"][i]["id"].as<u64>(0) != id;
           ++i) {
      }

      if (i < node["entities"].size()) {
        node["entities"][i] = updatedNode.getData();
      }
    }
  }
  updateCache.insert_or_assign(entity.raw_id(), true);
  mEntityIdCache.insert_or_assign(id, entity);
}

void SceneWriter::deleteEntities(const std::vector<Entity> &entities) {
  std::unordered_map<flecs::id_t, bool> deleteCache;

  for (auto entity : entities) {
    removeEntityFromSceneYaml(entity, mRoot, deleteCache);
  }

  save();
}

void SceneWriter::syncScene() {
  updateStartingCamera();
  updateEnvironment();
  save();
}

void SceneWriter::removeEntityFromSceneYaml(
    Entity entity, YAML::Node &node,
    std::unordered_map<flecs::id_t, bool> &deleteCache) {
  if (deleteCache.contains(entity)) {
    return;
  }

  if (entity.has<Children>()) {
    auto &children = entity.get_ref<Children>()->children;
    for (auto entity : children) {
      removeEntityFromSceneYaml(entity, node, deleteCache);
    }
  }

  if (!entity.has<Id>()) {
    return;
  }

  auto id = entity.get_ref<Id>()->id;

  usize i = 0;
  for (; i < node["entities"].size() &&
         node["entities"][i]["id"].as<u64>(0) != id;
       ++i) {
  }

  node["entities"].remove(i);

  if (mRoot["zones"][0]["startingCamera"].as<u64>(0) == id) {
    mRoot["zones"][0]["startingCamera"] = YAML::Node(YAML::NodeType::Null);
  }

  if (mRoot["zones"][0]["environment"].as<u64>(0) == id) {
    mRoot["zones"][0]["environment"] = YAML::Node(YAML::NodeType::Null);
  }

  mEntityIdCache.erase(id);
  deleteCache.insert_or_assign(entity.raw_id(), true);
}

void SceneWriter::updateStartingCamera() {
  if (!mScene.activeCamera.is_valid() || !mScene.activeCamera.has<Id>() ||
      !mScene.activeCamera.has<PerspectiveLens>()) {
    return;
  }

  mRoot["zones"][0]["startingCamera"] = mScene.activeCamera.get<Id>()->id;
}

void SceneWriter::updateEnvironment() {
  if (!mScene.activeEnvironment.is_valid() ||
      !mScene.activeEnvironment.has<Id>()) {
    return;
  }

  mRoot["zones"][0]["environment"] = mScene.activeEnvironment.get<Id>()->id;
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

u64 SceneWriter::generateId() { return mLastId++; }

} // namespace quoll::editor
