#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"

#include "EntityManager.h"

namespace liquidator {

EntityManager::EntityManager(liquid::AssetManager &assetManager,
                             liquid::Renderer &renderer,
                             const std::filesystem::path &scenePath)
    : mScenePath(scenePath), mRenderer(renderer), mAssetManager(assetManager),
      mSceneIO(mAssetManager.getRegistry(), mEntityDatabase) {}

void EntityManager::save(liquid::Entity entity) {
  if (mInSimulation)
    return;

  mSceneIO.saveEntity(entity, mScenePath);
}

liquid::Entity EntityManager::createEmptyEntity(
    liquid::Entity parent, const liquid::LocalTransformComponent &transform,
    const liquid::String &name) {

  auto &entityDatabase = getActiveEntityDatabase();

  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent(entity, transform);
  entityDatabase.setComponent<liquid::WorldTransformComponent>(entity, {});

  if (entityDatabase.hasEntity(parent)) {
    entityDatabase.setComponent<liquid::ParentComponent>(entity, {parent});

    if (!entityDatabase.hasComponent<liquid::ChildrenComponent>(parent)) {
      entityDatabase.setComponent<liquid::ChildrenComponent>(parent, {});
    }

    entityDatabase.getComponent<liquid::ChildrenComponent>(parent)
        .children.push_back(entity);
  }

  setName(entity, name);

  return entity;
}

liquid::Entity EntityManager::createEmptyEntity(EditorCamera &camera,
                                                liquid::Entity parent,
                                                const liquid::String &name,
                                                bool saveToFile) {
  auto entity = createEmptyEntity(parent, getTransformFromCamera(camera), name);

  if (saveToFile) {
    save(entity);
  }

  return entity;
}

bool EntityManager::loadScene() {
  mSceneIO.loadScene(mScenePath);

  return true;
}

void EntityManager::setSkeletonForEntity(liquid::Entity entity,
                                         liquid::SkeletonAssetHandle handle) {
  const auto &skeleton =
      mAssetManager.getRegistry().getSkeletons().getAsset(handle).data;

  liquid::SkeletonComponent skeletonInstance{};
  skeletonInstance.jointLocalPositions = skeleton.jointLocalPositions;
  skeletonInstance.jointLocalRotations = skeleton.jointLocalRotations;
  skeletonInstance.jointLocalScales = skeleton.jointLocalScales;
  skeletonInstance.jointParents = skeleton.jointParents;
  skeletonInstance.jointInverseBindMatrices = skeleton.jointInverseBindMatrices;
  skeletonInstance.jointNames = skeleton.jointNames;
  skeletonInstance.assetHandle = handle;
  skeletonInstance.numJoints =
      static_cast<uint32_t>(skeleton.jointLocalPositions.size());
  skeletonInstance.jointFinalTransforms.resize(skeletonInstance.numJoints,
                                               glm::mat4{1.0f});
  skeletonInstance.jointWorldTransforms.resize(skeletonInstance.numJoints,
                                               glm::mat4{1.0f});

  getActiveEntityDatabase().setComponent(entity, skeletonInstance);
}

void EntityManager::toggleSkeletonDebugForEntity(liquid::Entity entity) {
  auto &entityDatabase = getActiveEntityDatabase();
  if (!entityDatabase.hasComponent<liquid::SkeletonComponent>(entity)) {
    return;
  }

  if (entityDatabase.hasComponent<liquid::SkeletonDebugComponent>(entity)) {
    entityDatabase.deleteComponent<liquid::SkeletonDebugComponent>(entity);
    return;
  }

  auto &skeleton =
      entityDatabase.getComponent<liquid::SkeletonComponent>(entity);

  liquid::SkeletonDebugComponent skeletonDebug{};
  auto numBones = skeleton.numJoints * 2;
  skeletonDebug.bones.reserve(numBones);

  for (uint32_t joint = 0; joint < skeleton.numJoints; ++joint) {
    skeletonDebug.bones.push_back(skeleton.jointParents.at(joint));
    skeletonDebug.bones.push_back(joint);
  }

  skeletonDebug.boneTransforms.resize(numBones, glm::mat4{1.0f});

  entityDatabase.setComponent(entity, skeletonDebug);
}

void EntityManager::setMesh(liquid::Entity entity,
                            liquid::MeshAssetHandle handle) {
  if (getActiveEntityDatabase().hasComponent<liquid::SkinnedMeshComponent>(
          entity)) {
    getActiveEntityDatabase().deleteComponent<liquid::SkinnedMeshComponent>(
        entity);
  }

  getActiveEntityDatabase().setComponent<liquid::MeshComponent>(entity,
                                                                {handle});
}

void EntityManager::setSkinnedMesh(liquid::Entity entity,
                                   liquid::SkinnedMeshAssetHandle handle) {
  if (getActiveEntityDatabase().hasComponent<liquid::MeshComponent>(entity)) {
    getActiveEntityDatabase().deleteComponent<liquid::MeshComponent>(entity);
  }
  getActiveEntityDatabase().setComponent<liquid::SkinnedMeshComponent>(
      entity, {handle});
}

void EntityManager::setName(liquid::Entity entity, const liquid::String &name) {
  if (name.empty()) {
    return;
  }

  getActiveEntityDatabase().setComponent<liquid::NameComponent>(entity, {name});
}

void EntityManager::setCamera(liquid::Entity entity,
                              const liquid::PerspectiveLensComponent &lens,
                              bool autoRatio) {
  getActiveEntityDatabase().setComponent<liquid::CameraComponent>(entity, {});
  getActiveEntityDatabase().setComponent<liquid::PerspectiveLensComponent>(
      entity, lens);
  if (autoRatio) {
    getActiveEntityDatabase().setComponent<liquid::AutoAspectRatioComponent>(
        entity, {});
  }
}

void EntityManager::setAudio(liquid::Entity entity,
                             liquid::AudioAssetHandle source) {
  getActiveEntityDatabase().setComponent<liquid::AudioSourceComponent>(
      entity, {source});
}

void EntityManager::setText(liquid::Entity entity, liquid::TextComponent text) {
  getActiveEntityDatabase().setComponent(entity, text);
}

void EntityManager::setScript(liquid::Entity entity,
                              liquid::LuaScriptAssetHandle handle) {
  liquid::ScriptingComponent script{};
  script.handle = handle;
  getActiveEntityDatabase().setComponent(entity, script);
}

void EntityManager::deleteEntity(liquid::Entity entity) {
  if (!mInSimulation) {
    mSceneIO.deleteEntityFilesAndRelations(entity, mScenePath);
  }

  getActiveEntityDatabase().setComponent<liquid::DeleteComponent>(entity, {});
}

liquid::Entity EntityManager::spawnEntity(EditorCamera &camera,
                                          liquid::Entity root, uint32_t handle,
                                          liquid::AssetType type,
                                          bool saveToFile) {
  if (type != liquid::AssetType::Prefab) {
    return liquid::EntityNull;
  }

  auto &asset = mAssetManager.getRegistry().getPrefabs().getAsset(
      static_cast<liquid::PrefabAssetHandle>(handle));
  auto parent = createEmptyEntity(camera, root, asset.name, saveToFile);

  uint32_t childIndex = 1;
  std::unordered_map<uint32_t, liquid::Entity> entityMap;

  auto getOrCreateEntity =
      [&entityMap, this, parent, &camera, &childIndex](
          uint32_t localId,
          const liquid::LocalTransformComponent &transform = {}) mutable {
        if (entityMap.find(localId) == entityMap.end()) {
          auto entity = createEmptyEntity(
              parent, transform, "Untitled " + std::to_string(childIndex));
          entityMap.insert_or_assign(localId, entity);
          childIndex++;
        }

        return entityMap.at(localId);
      };

  for (auto &item : asset.data.transforms) {
    liquid::LocalTransformComponent transform{};
    transform.localPosition = item.value.position;
    transform.localRotation = item.value.rotation;
    transform.localScale = item.value.scale;

    getOrCreateEntity(item.entity, transform);
  }

  for (auto &item : asset.data.meshes) {
    if (!mAssetManager.getRegistry().getMeshes().hasAsset(item.value)) {
      continue;
    }

    setMesh(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.skinnedMeshes) {
    if (!mAssetManager.getRegistry().getSkinnedMeshes().hasAsset(item.value)) {
      continue;
    }

    setSkinnedMesh(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.skeletons) {
    if (!mAssetManager.getRegistry().getSkeletons().hasAsset(item.value)) {
      continue;
    }

    setSkeletonForEntity(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.animators) {
    auto entity = getOrCreateEntity(item.entity);
    getActiveEntityDatabase().setComponent(entity, item.value);
  }

  if (saveToFile) {
    for (auto [_, entity] : entityMap) {
      save(entity);
    }
  }

  return parent;
}

void EntityManager::updateSimulationEntityDatabase() {
  mSimulationEntityDatabase.destroy();
  mEntityDatabase.duplicate(mSimulationEntityDatabase);
}

void EntityManager::useSimulationDatabase() {
  updateSimulationEntityDatabase();
  mInSimulation = true;
}

void EntityManager::useEditingDatabase() { mInSimulation = false; }

liquid::LocalTransformComponent
EntityManager::getTransformFromCamera(EditorCamera &camera) const {
  auto &entityDatabase =
      mInSimulation ? mSimulationEntityDatabase : mEntityDatabase;
  const auto &viewMatrix =
      entityDatabase.getComponent<liquid::CameraComponent>(camera.getCamera())
          .viewMatrix;

  static constexpr glm::vec3 DistanceFromEye{0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(DistanceFromEye);

  liquid::LocalTransformComponent transform;
  transform.localPosition = orientation[3];
  return transform;
}

} // namespace liquidator
