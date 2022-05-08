#include "liquid/core/Base.h"
#include "liquid/yaml/Yaml.h"

#include "EntityManager.h"

namespace liquidator {

EntityManager::EntityManager(liquid::EntityContext &entityContext,
                             liquid::AssetManager &assetManager,
                             liquid::Renderer &renderer,
                             const std::filesystem::path &scenePath)
    : mEntityContext(entityContext), mScenePath(scenePath), mRenderer(renderer),
      mAssetManager(assetManager) {}

void EntityManager::save(liquid::Entity entity) {
  YAML::Node node;
  node["version"] = "0.1";
  node["id"] = mEntityContext.getComponent<liquid::IdComponent>(entity).id;

  if (mEntityContext.hasComponent<liquid::NameComponent>(entity)) {
    const auto &name =
        mEntityContext.getComponent<liquid::NameComponent>(entity).name;
    node["components"]["name"] = name;
  }

  if (mEntityContext.hasComponent<liquid::TransformComponent>(entity)) {
    const auto &transform =
        mEntityContext.getComponent<liquid::TransformComponent>(entity);

    node["components"]["transform"]["position"] = transform.localPosition;
    node["components"]["transform"]["rotation"] = transform.localRotation;
    node["components"]["transform"]["scale"] = transform.localScale;
    if (mEntityContext.hasComponent<liquid::TransformComponent>(
            transform.parent) &&
        mEntityContext.hasComponent<liquid::IdComponent>(transform.parent)) {
      auto parentId =
          mEntityContext.getComponent<liquid::IdComponent>(transform.parent).id;

      node["components"]["transform"]["parent"] = parentId;
    } else {
      node["components"]["transform"]["parent"] = 0;
    }
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
                      .skeleton.getAssetHandle();

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

liquid::SceneNode *
EntityManager::createEmptyEntity(liquid::SceneNode *parent,
                                 const liquid::TransformComponent &transform,
                                 const liquid::String &name) {
  auto entity = mEntityContext.createEntity();
  mEntityContext.setComponent<liquid::DebugComponent>(entity, {});
  mEntityContext.setComponent<liquid::IdComponent>(entity, {mLastId});
  setName(entity, name);

  auto *node = parent->addChild(entity, transform);

  mLastId++;

  return node;
}

liquid::SceneNode *EntityManager::createEmptyEntity(EditorCamera &camera,
                                                    liquid::SceneNode *parent,
                                                    const liquid::String &name,
                                                    bool saveToFile) {
  auto *node = createEmptyEntity(parent, getTransformFromCamera(camera), name);

  if (saveToFile) {
    save(node->getEntity());
  }

  return node;
}

bool EntityManager::loadScene(liquid::SceneNode *root) {
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

  std::map<uint64_t, liquid::SceneNode *> entityMap;

  for (auto &[id, node] : mapping) {
    auto entity = mEntityContext.createEntity();
    liquid::IdComponent idComponent = {node["id"].as<uint64_t>()};
    mEntityContext.setComponent(entity, idComponent);
    entityMap.insert_or_assign(
        idComponent.id,
        new liquid::SceneNode(entity, {}, nullptr, mEntityContext));

    mLastId = std::max(idComponent.id, mLastId);

    liquid::String name;
    if (node["components"]["name"].IsScalar()) {
      name = node["components"]["name"].as<liquid::String>();
    }
    setName(entity, name);

    if (node["components"]["transform"].IsMap()) {
      auto parsed = node["components"]["transform"];
      liquid::TransformComponent component{};
      if (parsed["position"].IsSequence()) {
        component.localPosition = parsed["position"].as<glm::vec3>();
      }

      if (parsed["rotation"].IsSequence()) {
        component.localRotation = parsed["rotation"].as<glm::quat>();
      }

      if (parsed["scale"].IsSequence()) {
        component.localScale = parsed["scale"].as<glm::vec3>();
      }

      mEntityContext.setComponent(entity, component);
    }

    if (node["components"]["mesh"].IsScalar()) {
      auto relativePathStr = node["components"]["mesh"].as<liquid::String>();
      auto relativePath = std::filesystem::path(relativePathStr);
      auto handle =
          mAssetManager.getRegistry().getMeshes().findHandleByRelativePath(
              relativePath);
      setMeshForEntity(entity, handle);
    } else if (node["components"]["skinnedMesh"].IsScalar()) {
      auto relativePathStr =
          node["components"]["skinnedMesh"].as<liquid::String>();
      auto relativePath = std::filesystem::path(relativePathStr);
      auto handle = mAssetManager.getRegistry()
                        .getSkinnedMeshes()
                        .findHandleByRelativePath(relativePath);
      setSkinnedMeshForEntity(entity, handle);
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
        mEntityContext.setComponent(entity, component);
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

      createCamera(entity, component, autoRatio);
    }

    mEntityContext.setComponent<liquid::DebugComponent>(entity, {});
  }

  for (auto &[id, node] : mapping) {
    if (node["components"]["transform"]["parent"].IsScalar()) {
      auto *sceneNode = entityMap.at(id);
      auto &transform = sceneNode->getTransform();
      auto parent = node["components"]["transform"]["parent"].as<uint64_t>();

      if (entityMap.find(parent) != entityMap.end()) {
        transform.parent = entityMap.at(parent)->getEntity();
        entityMap.at(parent)->addChild(sceneNode);
      } else {
        root->addChild(sceneNode);
      }
    }
  }

  mLastId++;

  return true;
}

void EntityManager::setSkeletonForEntity(liquid::Entity entity,
                                         liquid::SkeletonAssetHandle handle) {
  const auto &skeleton =
      mAssetManager.getRegistry().getSkeletons().getAsset(handle).data;

  liquid::Skeleton skeletonInstance(
      handle, skeleton.jointLocalPositions, skeleton.jointLocalRotations,
      skeleton.jointLocalScales, skeleton.jointParents,
      skeleton.jointInverseBindMatrices, skeleton.jointNames,
      &mRenderer.getRegistry());
  mEntityContext.setComponent<liquid::SkeletonComponent>(
      entity, {std::move(skeletonInstance)});
}

void EntityManager::setMeshForEntity(liquid::Entity entity,
                                     liquid::MeshAssetHandle handle) {
  mEntityContext.setComponent<liquid::MeshComponent>(entity, {handle});
}

void EntityManager::setSkinnedMeshForEntity(
    liquid::Entity entity, liquid::SkinnedMeshAssetHandle handle) {
  if (mEntityContext.hasComponent<liquid::MeshComponent>(entity)) {
    mEntityContext.deleteComponent<liquid::MeshComponent>(entity);
  }
  mEntityContext.setComponent<liquid::SkinnedMeshComponent>(entity, {handle});
}

void EntityManager::setName(liquid::Entity entity, const liquid::String &name) {
  if (!mEntityContext.hasComponent<liquid::NameComponent>(entity)) {
    liquid::NameComponent component{};
    mEntityContext.setComponent(entity, component);
  }

  auto &component = mEntityContext.getComponent<liquid::NameComponent>(entity);

  if (name.empty()) {
    component.name =
        "Untitled " +
        std::to_string(
            mEntityContext.getComponent<liquid::IdComponent>(entity).id);
  } else {
    component.name = name;
  }
}

void EntityManager::createCamera(liquid::Entity entity,
                                 const liquid::PerspectiveLensComponent &lens,
                                 bool autoRatio) {
  mEntityContext.setComponent<liquid::CameraComponent>(entity, {});
  mEntityContext.setComponent<liquid::PerspectiveLensComponent>(entity, lens);
  if (autoRatio) {
    mEntityContext.setComponent<liquid::AutoAspectRatioComponent>(entity, {});
  }
}

void EntityManager::deleteEntity(liquid::Entity entity) {
  auto id = mEntityContext.getComponent<liquid::IdComponent>(entity);

  auto fileName = std::to_string(id.id) + ".lqnode";
  mEntityContext.deleteEntity(entity);
  std::filesystem::remove(std::filesystem::path(mScenePath / fileName));
}

liquid::Entity EntityManager::spawnAsset(EditorCamera &camera,
                                         liquid::SceneNode *root,
                                         uint32_t handle,
                                         liquid::AssetType type,
                                         bool saveToFile) {
  if (type != liquid::AssetType::Prefab) {
    return liquid::ENTITY_MAX;
  }

  auto &asset = mAssetManager.getRegistry().getPrefabs().getAsset(
      static_cast<liquid::PrefabAssetHandle>(handle));
  auto *parent = createEmptyEntity(camera, root, asset.name, saveToFile);
  auto parentEntity = parent->getEntity();

  std::map<uint32_t, liquid::Entity> entityMap;

  auto getOrCreateEntity =
      [&entityMap, this, &camera,
       &parent](uint32_t localId,
                const liquid::TransformComponent &transform = {}) mutable {
        if (entityMap.find(localId) == entityMap.end()) {
          auto *node = createEmptyEntity(parent, transform);
          entityMap.insert_or_assign(localId, node->getEntity());
        }

        return entityMap.at(localId);
      };

  for (auto &item : asset.data.transforms) {
    liquid::TransformComponent transform{};
    transform.localPosition = item.value.position;
    transform.localRotation = item.value.rotation;
    transform.localScale = item.value.scale;
    getOrCreateEntity(item.entity, transform);
  }

  for (auto &item : asset.data.transforms) {
    auto &transform = mEntityContext.getComponent<liquid::TransformComponent>(
        entityMap.at(item.entity));
    if (item.value.parent >= 0) {
      transform.parent = getOrCreateEntity(item.value.parent);
    }
  }

  for (auto &item : asset.data.meshes) {
    if (!mAssetManager.getRegistry().getMeshes().hasAsset(item.value)) {
      continue;
    }

    setMeshForEntity(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.skinnedMeshes) {
    if (!mAssetManager.getRegistry().getSkinnedMeshes().hasAsset(item.value)) {
      continue;
    }

    setSkinnedMeshForEntity(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.skeletons) {
    if (!mAssetManager.getRegistry().getSkeletons().hasAsset(item.value)) {
      continue;
    }

    setSkeletonForEntity(getOrCreateEntity(item.entity), item.value);
  }

  for (auto &item : asset.data.animators) {
    auto entity = getOrCreateEntity(item.entity);
    mEntityContext.setComponent(entity, item.value);
  }

  if (saveToFile) {
    for (auto [_, entity] : entityMap) {
      save(entity);
    }
  }

  return parentEntity;
}

liquid::TransformComponent
EntityManager::getTransformFromCamera(EditorCamera &camera) const {
  const auto &viewMatrix =
      mEntityContext.getComponent<liquid::CameraComponent>(camera.getCamera())
          .viewMatrix;

  constexpr glm::vec3 distanceFromEye = {0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(distanceFromEye);

  liquid::TransformComponent transform;
  transform.localPosition = orientation[3];
  return transform;
}

} // namespace liquidator
