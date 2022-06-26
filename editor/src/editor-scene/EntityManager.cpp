#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"

#include "EntityManager.h"

namespace liquidator {

EntityManager::EntityManager(liquid::AssetManager &assetManager,
                             liquid::Renderer &renderer,
                             const std::filesystem::path &scenePath)
    : mScenePath(scenePath), mRenderer(renderer), mAssetManager(assetManager) {}

void EntityManager::save(liquid::Entity entity) {
  if (mInSimulation)
    return;

  auto entityId = mEntityDatabase.getComponent<liquid::IdComponent>(entity).id;

  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = entityId;

  if (mEntityDatabase.hasComponent<liquid::NameComponent>(entity)) {
    const auto &name =
        mEntityDatabase.getComponent<liquid::NameComponent>(entity).name;
    node["components"]["name"] = name;
  }

  if (mEntityDatabase.hasComponent<liquid::LocalTransformComponent>(entity)) {
    const auto &transform =
        mEntityDatabase.getComponent<liquid::LocalTransformComponent>(entity);

    node["components"]["transform"]["position"] = transform.localPosition;
    node["components"]["transform"]["rotation"] = transform.localRotation;
    node["components"]["transform"]["scale"] = transform.localScale;
  }

  if (mEntityDatabase.hasComponent<liquid::ParentComponent>(entity)) {
    node["components"]["transform"]["parent"] =
        mEntityDatabase.getComponent<liquid::ParentComponent>(entity).parent;
  } else {
    node["components"]["transform"]["parent"] = 0;
  }

  if (mEntityDatabase.hasComponent<liquid::MeshComponent>(entity)) {
    auto handle =
        mEntityDatabase.getComponent<liquid::MeshComponent>(entity).handle;

    node["components"]["mesh"] = mAssetManager.getRegistry()
                                     .getMeshes()
                                     .getAsset(handle)
                                     .relativePath.string();
  } else if (mEntityDatabase.hasComponent<liquid::SkinnedMeshComponent>(
                 entity)) {
    auto handle =
        mEntityDatabase.getComponent<liquid::SkinnedMeshComponent>(entity)
            .handle;
    node["components"]["skinnedMesh"] = mAssetManager.getRegistry()
                                            .getSkinnedMeshes()
                                            .getAsset(handle)
                                            .relativePath.string();
  }

  if (mEntityDatabase.hasComponent<liquid::SkeletonComponent>(entity)) {
    auto handle =
        mEntityDatabase.getComponent<liquid::SkeletonComponent>(entity)
            .assetHandle;

    node["components"]["skeleton"] = mAssetManager.getRegistry()
                                         .getSkeletons()
                                         .getAsset(handle)
                                         .relativePath.string();
  }

  if (mEntityDatabase.hasComponent<liquid::DirectionalLightComponent>(entity)) {
    const auto &light =
        mEntityDatabase.getComponent<liquid::DirectionalLightComponent>(entity);

    // Directional light
    node["components"]["light"]["type"] = 0;
    node["components"]["light"]["color"] = light.color;
    node["components"]["light"]["intensity"] = light.intensity;
  }

  if (mEntityDatabase.hasComponent<liquid::PerspectiveLensComponent>(entity)) {
    const auto &lens =
        mEntityDatabase.getComponent<liquid::PerspectiveLensComponent>(entity);

    node["components"]["camera"]["type"] = 0; // ZERO = Perspective
    node["components"]["camera"]["fov"] = lens.fovY;
    node["components"]["camera"]["near"] = lens.near;
    node["components"]["camera"]["far"] = lens.far;

    if (mEntityDatabase.hasComponent<liquid::AutoAspectRatioComponent>(
            entity)) {
      node["components"]["camera"]["aspectRatio"] = "auto";
    } else {
      node["components"]["camera"]["aspectRatio"] = lens.aspectRatio;
    }
  }

  if (mEntityDatabase.hasComponent<liquid::ScriptingComponent>(entity)) {
    const auto &script =
        mEntityDatabase.getComponent<liquid::ScriptingComponent>(entity);

    node["components"]["script"] = mAssetManager.getRegistry()
                                       .getLuaScripts()
                                       .getAsset(script.handle)
                                       .relativePath.string();
  }

  if (mEntityDatabase.hasComponent<liquid::AudioSourceComponent>(entity)) {
    const auto audioSource =
        mEntityDatabase.getComponent<liquid::AudioSourceComponent>(entity);

    node["components"]["audio"]["source"] = mAssetManager.getRegistry()
                                                .getAudios()
                                                .getAsset(audioSource.source)
                                                .relativePath.string();
  }

  auto fileName = std::to_string(entityId) + ".lqnode";
  std::ofstream out(mScenePath / fileName, std::ios::out);
  out << node;
  out.close();
}

liquid::Entity EntityManager::createEmptyEntity(
    liquid::Entity parent, const liquid::LocalTransformComponent &transform,
    const liquid::String &name) {
  auto entity = getActiveEntityDatabase().createEntity();
  getActiveEntityDatabase().setComponent<liquid::IdComponent>(entity,
                                                              {mLastId});
  getActiveEntityDatabase().setComponent(entity, transform);
  getActiveEntityDatabase().setComponent<liquid::WorldTransformComponent>(
      entity, {});

  if (getActiveEntityDatabase().hasEntity(parent)) {
    getActiveEntityDatabase().setComponent<liquid::ParentComponent>(entity,
                                                                    {parent});

    if (!getActiveEntityDatabase().hasComponent<liquid::ChildrenComponent>(
            parent)) {
      getActiveEntityDatabase().setComponent<liquid::ChildrenComponent>(parent,
                                                                        {});
    }

    getActiveEntityDatabase()
        .getComponent<liquid::ChildrenComponent>(parent)
        .children.push_back(entity);
  }

  setName(entity, name);

  mLastId++;

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
  std::unordered_map<uint32_t, YAML::Node> mapping;
  for (auto entry : std::filesystem::directory_iterator(mScenePath)) {
    std::ifstream stream(entry.path(), std::ios::in);
    auto node = YAML::Load(stream);
    stream.close();

    if (node["id"].IsScalar() && node["components"].IsMap()) {
      mapping.insert_or_assign(node["id"].as<uint32_t>(), node);
    }
  }

  if (mapping.empty()) {
    return false;
  }

  std::map<uint64_t, liquid::Entity> newEntityMap;

  for (auto &[id, node] : mapping) {
    auto entity = getActiveEntityDatabase().createEntity();
    liquid::IdComponent idComponent = {node["id"].as<uint64_t>()};
    getActiveEntityDatabase().setComponent(entity, idComponent);
    newEntityMap.insert_or_assign(idComponent.id, entity);

    mLastId = std::max(idComponent.id, mLastId);

    liquid::String name;
    if (node["components"]["name"].IsScalar()) {
      name = node["components"]["name"].as<liquid::String>();
    }
    setName(entity, name);

    if (node["components"]["transform"].IsMap()) {
      auto parsed = node["components"]["transform"];
      liquid::LocalTransformComponent component{};
      if (parsed["position"].IsSequence()) {
        component.localPosition = parsed["position"].as<glm::vec3>();
      }

      if (parsed["rotation"].IsSequence()) {
        component.localRotation = parsed["rotation"].as<glm::quat>();
      }

      if (parsed["scale"].IsSequence()) {
        component.localScale = parsed["scale"].as<glm::vec3>();
      }

      getActiveEntityDatabase().setComponent(entity, component);
      getActiveEntityDatabase().setComponent<liquid::WorldTransformComponent>(
          entity, {});
    }

    if (node["components"]["mesh"].IsScalar()) {
      auto relativePathStr = node["components"]["mesh"].as<liquid::String>();
      auto relativePath = std::filesystem::path(relativePathStr);
      auto handle =
          mAssetManager.getRegistry().getMeshes().findHandleByRelativePath(
              relativePath);
      setMesh(entity, handle);
    } else if (node["components"]["skinnedMesh"].IsScalar()) {
      auto relativePathStr =
          node["components"]["skinnedMesh"].as<liquid::String>();
      auto relativePath = std::filesystem::path(relativePathStr);
      auto handle = mAssetManager.getRegistry()
                        .getSkinnedMeshes()
                        .findHandleByRelativePath(relativePath);
      setSkinnedMesh(entity, handle);
    }

    if (node["components"]["skeleton"].IsScalar()) {
      auto relativePathStr =
          node["components"]["skeleton"].as<liquid::String>();
      auto relativePath = std::filesystem::path(relativePathStr);
      auto handle =
          mAssetManager.getRegistry().getSkeletons().findHandleByRelativePath(
              relativePath);
      setSkeletonForEntity(entity, handle);
    }

    if (node["components"]["light"].IsMap()) {
      float intensity = 1.0f;
      glm::vec4 color{1.0f};

      if (!node["components"]["light"]["type"].IsScalar()) {
        continue;
      }

      if (node["components"]["light"]["intensity"].IsScalar()) {
        intensity = node["components"]["light"]["intensity"].as<float>();
      }

      if (node["components"]["light"]["color"].IsScalar()) {
        color = node["components"]["light"]["color"].as<glm::vec4>();
      }

      auto type = node["components"]["light"]["type"].as<uint32_t>();

      // Directional
      if (type == 0) {
        liquid::DirectionalLightComponent component;
        component.color = color;
        component.intensity = intensity;
        getActiveEntityDatabase().setComponent(entity, component);
      }
    }

    if (node["components"]["camera"].IsMap()) {
      const auto &camera = node["components"]["camera"];
      bool autoRatio = false;

      liquid::PerspectiveLensComponent component;
      if (camera["fov"].IsScalar()) {
        component.fovY = camera["fov"].as<float>();
      }

      if (camera["near"].IsScalar()) {
        component.near = camera["near"].as<float>();
      }

      if (camera["far"].IsScalar()) {
        component.far = camera["far"].as<float>();
      }

      if (camera["aspectRatio"].IsScalar()) {
        autoRatio = camera["aspectRatio"].as<liquid::String>() == "auto";

        if (!autoRatio) {
          component.aspectRatio = camera["aspectRatio"].as<float>();
        }
      }

      setCamera(entity, component, autoRatio);
    }

    if (node["components"]["audio"]["source"].IsScalar()) {
      auto relativePathStr =
          node["components"]["audio"]["source"].as<liquid::String>();
      auto relativePath = std::filesystem::path(relativePathStr);

      auto handle =
          mAssetManager.getRegistry().getAudios().findHandleByRelativePath(
              relativePath);

      setAudio(entity, handle);
    }

    if (node["components"]["script"].IsScalar()) {
      auto relativePathStr = node["components"]["script"].as<liquid::String>();
      auto relativePath = std::filesystem::path(relativePathStr);
      auto handle =
          mAssetManager.getRegistry().getLuaScripts().findHandleByRelativePath(
              relativePath);
      setScript(entity, handle);
    }
  }

  std::unordered_map<liquid::Entity, std::vector<liquid::Entity>> childrenMap;

  for (auto &[id, node] : mapping) {
    if (node["components"]["transform"]["parent"].IsScalar()) {
      auto parent = node["components"]["transform"]["parent"].as<uint64_t>();
      auto entity = newEntityMap.at(id);

      if (newEntityMap.find(parent) != newEntityMap.end()) {
        auto parentEntity = newEntityMap.at(parent);
        getActiveEntityDatabase().setComponent<liquid::ParentComponent>(
            entity, {newEntityMap.at(parent)});

        childrenMap[parentEntity].push_back(entity);
      }
    }
  }

  for (auto &[entity, children] : childrenMap) {
    getActiveEntityDatabase().setComponent<liquid::ChildrenComponent>(
        entity, {children});
  }

  mLastId++;

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
  if (!getActiveEntityDatabase().hasComponent<liquid::NameComponent>(entity)) {
    liquid::NameComponent component{};
    getActiveEntityDatabase().setComponent(entity, component);
  }

  auto &component =
      getActiveEntityDatabase().getComponent<liquid::NameComponent>(entity);

  if (name.empty()) {
    component.name =
        "Untitled " +
        std::to_string(getActiveEntityDatabase()
                           .getComponent<liquid::IdComponent>(entity)
                           .id);
  } else {
    component.name = name;
  }
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

void EntityManager::setScript(liquid::Entity entity,
                              liquid::LuaScriptAssetHandle handle) {
  liquid::ScriptingComponent script{};
  script.handle = handle;
  getActiveEntityDatabase().setComponent(entity, script);
}

void EntityManager::deleteEntity(liquid::Entity entity) {
  auto id = getActiveEntityDatabase().getComponent<liquid::IdComponent>(entity);

  auto fileName = std::to_string(id.id) + ".lqnode";
  getActiveEntityDatabase().setComponent<liquid::DeleteComponent>(entity, {});

  if (!mInSimulation) {
    std::filesystem::remove(std::filesystem::path(mScenePath / fileName));
  }

  if (getActiveEntityDatabase().hasComponent<liquid::ChildrenComponent>(
          entity)) {
    for (auto child : getActiveEntityDatabase()
                          .getComponent<liquid::ChildrenComponent>(entity)
                          .children) {
      deleteEntity(child);
    }
  }
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

  std::map<uint32_t, liquid::Entity> entityMap;

  auto getOrCreateEntity =
      [&entityMap, this, parent,
       &camera](uint32_t localId,
                const liquid::LocalTransformComponent &transform = {}) mutable {
        if (entityMap.find(localId) == entityMap.end()) {
          auto entity = createEmptyEntity(parent, transform);
          entityMap.insert_or_assign(localId, entity);
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

  constexpr glm::vec3 distanceFromEye = {0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(distanceFromEye);

  liquid::LocalTransformComponent transform;
  transform.localPosition = orientation[3];
  return transform;
}

} // namespace liquidator
