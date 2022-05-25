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

  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = mEntityContext.getComponent<liquid::IdComponent>(entity).id;

  if (mEntityContext.hasComponent<liquid::NameComponent>(entity)) {
    const auto &name =
        mEntityContext.getComponent<liquid::NameComponent>(entity).name;
    node["components"]["name"] = name;
  }

  if (mEntityContext.hasComponent<liquid::LocalTransformComponent>(entity)) {
    const auto &transform =
        mEntityContext.getComponent<liquid::LocalTransformComponent>(entity);

    node["components"]["transform"]["position"] = transform.localPosition;
    node["components"]["transform"]["rotation"] = transform.localRotation;
    node["components"]["transform"]["scale"] = transform.localScale;
  }

  if (mEntityContext.hasComponent<liquid::ParentComponent>(entity)) {
    node["components"]["transform"]["parent"] =
        mEntityContext.getComponent<liquid::ParentComponent>(entity).parent;
  } else {
    node["components"]["transform"]["parent"] = 0;
  }

  if (mEntityContext.hasComponent<liquid::MeshComponent>(entity)) {
    auto handle =
        mEntityContext.getComponent<liquid::MeshComponent>(entity).handle;

    node["components"]["mesh"] = mAssetManager.getRegistry()
                                     .getMeshes()
                                     .getAsset(handle)
                                     .relativePath.string();
  } else if (mEntityContext.hasComponent<liquid::SkinnedMeshComponent>(
                 entity)) {
    auto handle =
        mEntityContext.getComponent<liquid::SkinnedMeshComponent>(entity)
            .handle;
    node["components"]["skinnedMesh"] = mAssetManager.getRegistry()
                                            .getSkinnedMeshes()
                                            .getAsset(handle)
                                            .relativePath.string();
  }

  if (mEntityContext.hasComponent<liquid::SkeletonComponent>(entity)) {
    auto handle = mEntityContext.getComponent<liquid::SkeletonComponent>(entity)
                      .assetHandle;

    node["components"]["skeleton"] = mAssetManager.getRegistry()
                                         .getSkeletons()
                                         .getAsset(handle)
                                         .relativePath.string();
  }

  if (mEntityContext.hasComponent<liquid::DirectionalLightComponent>(entity)) {
    const auto &light =
        mEntityContext.getComponent<liquid::DirectionalLightComponent>(entity);

    // Directional light
    node["components"]["light"]["type"] = 0;
    node["components"]["light"]["color"] = light.color;
    node["components"]["light"]["intensity"] = light.intensity;
  }

  if (mEntityContext.hasComponent<liquid::PerspectiveLensComponent>(entity)) {
    const auto &lens =
        mEntityContext.getComponent<liquid::PerspectiveLensComponent>(entity);

    node["components"]["camera"]["type"] = 0; // ZERO = Perspective
    node["components"]["camera"]["fov"] = lens.fovY;
    node["components"]["camera"]["near"] = lens.near;
    node["components"]["camera"]["far"] = lens.far;

    if (mEntityContext.hasComponent<liquid::AutoAspectRatioComponent>(entity)) {
      node["components"]["camera"]["aspectRatio"] = "auto";
    } else {
      node["components"]["camera"]["aspectRatio"] = lens.aspectRatio;
    }
  }

  auto fileName = std::to_string(entity) + ".lqnode";
  std::ofstream out(mScenePath / fileName, std::ios::out);
  out << node;
  out.close();
}

liquid::Entity EntityManager::createEmptyEntity(
    liquid::Entity parent, const liquid::LocalTransformComponent &transform,
    const liquid::String &name) {
  auto entity = getActiveEntityContext().createEntity();
  getActiveEntityContext().setComponent<liquid::IdComponent>(entity, {mLastId});
  getActiveEntityContext().setComponent(entity, transform);
  getActiveEntityContext().setComponent<liquid::WorldTransformComponent>(entity,
                                                                         {});

  if (getActiveEntityContext().hasEntity(parent)) {
    getActiveEntityContext().setComponent<liquid::ParentComponent>(entity,
                                                                   {parent});

    if (!getActiveEntityContext().hasComponent<liquid::ChildrenComponent>(
            parent)) {
      getActiveEntityContext().setComponent<liquid::ChildrenComponent>(parent,
                                                                       {});
    }

    getActiveEntityContext()
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
    auto entity = getActiveEntityContext().createEntity();
    liquid::IdComponent idComponent = {node["id"].as<uint64_t>()};
    getActiveEntityContext().setComponent(entity, idComponent);
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

      getActiveEntityContext().setComponent(entity, component);
      getActiveEntityContext().setComponent<liquid::WorldTransformComponent>(
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
        getActiveEntityContext().setComponent(entity, component);
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
  }

  std::unordered_map<liquid::Entity, std::vector<liquid::Entity>> childrenMap;

  for (auto &[id, node] : mapping) {
    if (node["components"]["transform"]["parent"].IsScalar()) {
      auto parent = node["components"]["transform"]["parent"].as<uint64_t>();
      auto entity = newEntityMap.at(id);

      if (newEntityMap.find(parent) != newEntityMap.end()) {
        auto parentEntity = newEntityMap.at(parent);
        getActiveEntityContext().setComponent<liquid::ParentComponent>(
            entity, {newEntityMap.at(parent)});

        childrenMap[parentEntity].push_back(entity);
      }
    }
  }

  for (auto &[entity, children] : childrenMap) {
    getActiveEntityContext().setComponent<liquid::ChildrenComponent>(
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

  getActiveEntityContext().setComponent(entity, skeletonInstance);
}

void EntityManager::toggleSkeletonDebugForEntity(liquid::Entity entity) {
  auto &entityContext = getActiveEntityContext();
  if (!entityContext.hasComponent<liquid::SkeletonComponent>(entity)) {
    return;
  }

  if (entityContext.hasComponent<liquid::SkeletonDebugComponent>(entity)) {
    entityContext.deleteComponent<liquid::SkeletonDebugComponent>(entity);
    return;
  }

  auto &skeleton =
      entityContext.getComponent<liquid::SkeletonComponent>(entity);

  liquid::SkeletonDebugComponent skeletonDebug{};
  auto numBones = skeleton.numJoints * 2;
  skeletonDebug.bones.reserve(numBones);

  for (uint32_t joint = 0; joint < skeleton.numJoints; ++joint) {
    skeletonDebug.bones.push_back(skeleton.jointParents.at(joint));
    skeletonDebug.bones.push_back(joint);
  }

  skeletonDebug.boneTransforms.resize(numBones, glm::mat4{1.0f});

  entityContext.setComponent(entity, skeletonDebug);
}

void EntityManager::setMesh(liquid::Entity entity,
                            liquid::MeshAssetHandle handle) {
  if (getActiveEntityContext().hasComponent<liquid::SkinnedMeshComponent>(
          entity)) {
    getActiveEntityContext().deleteComponent<liquid::SkinnedMeshComponent>(
        entity);
  }

  getActiveEntityContext().setComponent<liquid::MeshComponent>(entity,
                                                               {handle});
}

void EntityManager::setSkinnedMesh(liquid::Entity entity,
                                   liquid::SkinnedMeshAssetHandle handle) {
  if (getActiveEntityContext().hasComponent<liquid::MeshComponent>(entity)) {
    getActiveEntityContext().deleteComponent<liquid::MeshComponent>(entity);
  }
  getActiveEntityContext().setComponent<liquid::SkinnedMeshComponent>(entity,
                                                                      {handle});
}

void EntityManager::setName(liquid::Entity entity, const liquid::String &name) {
  if (!getActiveEntityContext().hasComponent<liquid::NameComponent>(entity)) {
    liquid::NameComponent component{};
    getActiveEntityContext().setComponent(entity, component);
  }

  auto &component =
      getActiveEntityContext().getComponent<liquid::NameComponent>(entity);

  if (name.empty()) {
    component.name =
        "Untitled " +
        std::to_string(getActiveEntityContext()
                           .getComponent<liquid::IdComponent>(entity)
                           .id);
  } else {
    component.name = name;
  }
}

void EntityManager::setCamera(liquid::Entity entity,
                              const liquid::PerspectiveLensComponent &lens,
                              bool autoRatio) {
  getActiveEntityContext().setComponent<liquid::CameraComponent>(entity, {});
  getActiveEntityContext().setComponent<liquid::PerspectiveLensComponent>(
      entity, lens);
  if (autoRatio) {
    getActiveEntityContext().setComponent<liquid::AutoAspectRatioComponent>(
        entity, {});
  }
}

void EntityManager::deleteEntity(liquid::Entity entity) {
  auto id = getActiveEntityContext().getComponent<liquid::IdComponent>(entity);

  auto fileName = std::to_string(id.id) + ".lqnode";
  getActiveEntityContext().deleteEntity(entity);

  if (mInSimulation) {
    std::filesystem::remove(std::filesystem::path(mScenePath / fileName));
  }

  if (getActiveEntityContext().hasComponent<liquid::ChildrenComponent>(
          entity)) {
    for (auto child : getActiveEntityContext()
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
    getActiveEntityContext().setComponent(entity, item.value);
  }

  if (saveToFile) {
    for (auto [_, entity] : entityMap) {
      save(entity);
    }
  }

  return parent;
}

void EntityManager::updateSimulationEntityContext() {
  mSimulationEntityContext.destroy();
  mEntityContext.duplicate(mSimulationEntityContext);
}

void EntityManager::useSimulationContext() {
  updateSimulationEntityContext();
  mInSimulation = true;
}

void EntityManager::useEditingContext() { mInSimulation = false; }

liquid::LocalTransformComponent
EntityManager::getTransformFromCamera(EditorCamera &camera) const {
  auto &entityContext =
      mInSimulation ? mSimulationEntityContext : mEntityContext;
  const auto &viewMatrix =
      entityContext.getComponent<liquid::CameraComponent>(camera.getCamera())
          .viewMatrix;

  constexpr glm::vec3 distanceFromEye = {0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(distanceFromEye);

  liquid::LocalTransformComponent transform;
  transform.localPosition = orientation[3];
  return transform;
}

} // namespace liquidator
