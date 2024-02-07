#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/scene/Scene.h"
#include "quoll/yaml/Yaml.h"
#include "quoll/editor/asset/AssetSyncer.h"

namespace quoll::editor {

class SceneWriter : public AssetSyncer, NoCopyMove {
public:
  SceneWriter(Scene &scene, AssetRegistry &assetRegistry);

  virtual ~SceneWriter() = default;

  void open(Path sourcePath);

  void syncEntities(const std::vector<Entity> &entities) override;

  void deleteEntities(const std::vector<Entity> &entities) override;

  void syncScene() override;

private:
  void updateSceneYaml(Entity entity, YAML::Node &node,
                       std::unordered_map<Entity, bool> &updateCache);

  void removeEntityFromSceneYaml(Entity entity, YAML::Node &node,
                                 std::unordered_map<Entity, bool> &deleteCache);

  void updateStartingCamera();

  void updateEnvironment();

  void save();

  u64 generateId();

private:
  Scene &mScene;
  Path mSourcePath;
  AssetRegistry &mAssetRegistry;

  std::fstream mStream;
  YAML::Node mRoot;

  std::unordered_map<u64, Entity> mEntityIdCache;

  u64 mLastId = 1;
};

} // namespace quoll::editor
