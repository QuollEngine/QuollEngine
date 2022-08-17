#include "liquid/core/Base.h"
#include "SceneIO.h"

namespace liquid {

SceneIO::SceneIO(AssetRegistry &assetRegistry, EntityDatabase &entityDatabase)
    : mEntityDatabase(entityDatabase), mLoader(assetRegistry, entityDatabase),
      mDeserializer(assetRegistry, entityDatabase) {}

std::vector<Entity> SceneIO::loadScene(const Path &path) {
  std::vector<Entity> entities;
  std::vector<YAML::Node> yamlNodes;

  if (std::filesystem::is_directory(path)) {
    for (const auto &entry :
         std::filesystem::recursive_directory_iterator(path)) {
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

    mLoader.loadComponents(node, entities.at(i), mEntityIdCache);
  }

  return entities;
}

void SceneIO::saveEntity(Entity entity, const Path &path) {
  if (!mEntityDatabase.has<IdComponent>(entity)) {
    mEntityDatabase.set<IdComponent>(entity, {generateId()});
  }

  auto id = mEntityDatabase.get<IdComponent>(entity).id;
  mEntityIdCache.insert({id, entity});

  auto node = mDeserializer.serialize(entity);

  if (node.hasData() && std::filesystem::is_directory(path)) {
    std::ofstream stream(getEntityPath(entity, path), std::ios::out);
    stream << node.getData();
    stream.close();
  }
}

void SceneIO::deleteEntityFilesAndRelations(Entity entity, const Path &path) {
  if (mEntityDatabase.has<ChildrenComponent>(entity)) {
    const auto &children = mEntityDatabase.get<ChildrenComponent>(entity);
    for (auto entity : children.children) {
      deleteEntityFilesAndRelations(entity, path);
    }
  }

  if (mEntityDatabase.has<IdComponent>(entity)) {
    auto id = mEntityDatabase.get<IdComponent>(entity).id;
    mEntityIdCache.insert({id, entity});

    std::filesystem::remove(getEntityPath(entity, path));
  }
}

Path SceneIO::getEntityPath(Entity entity, const Path &path) {
  auto id = mEntityDatabase.get<IdComponent>(entity).id;
  return path / (std::to_string(id) + ".lqnode");
}

uint64_t SceneIO::generateId() { return mLastId++; }

Result<Entity> SceneIO::createEntityFromNode(const YAML::Node &node) {
  if (node["id"] && node["id"].IsScalar()) {
    auto id = node["id"].as<uint64_t>(0);

    if (id > 0 && mEntityIdCache.find(id) == mEntityIdCache.end()) {
      auto entity = mEntityDatabase.create();
      mEntityDatabase.set<IdComponent>(entity, {id});

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
