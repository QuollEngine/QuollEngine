#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/core/Id.h"
#include "quoll/core/Name.h"
#include "quoll/animation/Animator.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/audio/AudioSource.h"
#include "quoll/audio/AudioStart.h"
#include "quoll/audio/AudioStatus.h"
#include "quoll/input/InputMap.h"
#include "quoll/physics/Collidable.h"
#include "quoll/physics/Force.h"
#include "quoll/physics/RigidBody.h"
#include "quoll/physics/RigidBodyClear.h"
#include "quoll/physics/Torque.h"
#include "quoll/physx/PhysxInstance.h"
#include "quoll/renderer/Mesh.h"
#include "quoll/renderer/MeshRenderer.h"
#include "quoll/renderer/SkinnedMeshRenderer.h"
#include "quoll/scene/AutoAspectRatio.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/CascadedShadowMap.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/DirectionalLight.h"
#include "quoll/scene/EnvironmentLighting.h"
#include "quoll/scene/EnvironmentSkybox.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/scene/PointLight.h"
#include "quoll/scene/Sprite.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/skeleton/JointAttachment.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll/text/Text.h"
#include "quoll/ui/UICanvas.h"
#include "quoll/ui/UICanvasRenderRequest.h"
#include "EntityDatabase.h"
#include "EntitySpawner.h"

namespace quoll {

EntitySpawner::EntitySpawner(EntityDatabase &entityDatabase,
                             AssetCache &assetCache)
    : mEntityDatabase(entityDatabase), mAssetCache(assetCache) {}

Entity EntitySpawner::spawnEmpty(LocalTransform transform) {
  auto entity = mEntityDatabase.create();
  mEntityDatabase.set(entity, transform);
  mEntityDatabase.set<WorldTransform>(entity, {});
  mEntityDatabase.set<Name>(entity, {"New entity"});
  return entity;
}

std::vector<Entity> EntitySpawner::spawnPrefab(AssetRef<PrefabAsset> prefab,
                                               LocalTransform transform) {
  QuollAssert(prefab, "Prefab not found");

  const auto &assetName = prefab.meta().name;
  const auto &asset = prefab.get();

  std::unordered_map<u32, usize> entityMap;
  std::vector<Entity> entities;

  auto getOrCreateEntity = [&entityMap, &entities, this](u32 localId) mutable {
    if (entityMap.find(localId) == entityMap.end()) {
      auto entity = mEntityDatabase.create();
      mEntityDatabase.set<LocalTransform>(entity, {});
      mEntityDatabase.set<WorldTransform>(entity, {});
      mEntityDatabase.set<Name>(entity, {"New entity"});

      entities.push_back(entity);
      entityMap.insert_or_assign(localId, entities.size() - 1);
      return entity;
    }

    return entities.at(entityMap.at(localId));
  };

  for (const auto &pTransform : asset.transforms) {
    if (pTransform.value.parent >= 0) {
      auto parent = getOrCreateEntity(pTransform.value.parent);
      auto entity = getOrCreateEntity(pTransform.entity);
      mEntityDatabase.set(entity, Parent{parent});

      if (!mEntityDatabase.has<Children>(parent)) {
        mEntityDatabase.set<Children>(parent, {{entity}});
      } else {
        mEntityDatabase.get<Children>(parent).children.push_back(entity);
      }
    }
  }

  for (const auto &pTransform : asset.transforms) {
    auto entity = getOrCreateEntity(pTransform.entity);
    LocalTransform transform{};
    transform.localPosition = pTransform.value.position;
    transform.localRotation = pTransform.value.rotation;
    transform.localScale = pTransform.value.scale;

    mEntityDatabase.set<LocalTransform>(entity, transform);
  }

  for (const auto &pName : asset.names) {
    auto entity = getOrCreateEntity(pName.entity);
    Name name{};
    name.name = pName.value;
    mEntityDatabase.set<Name>(entity, name);
  }

  for (const auto &pMesh : asset.meshes) {
    auto entity = getOrCreateEntity(pMesh.entity);
    mEntityDatabase.set<Mesh>(entity, {pMesh.value});
  }

  for (const auto &pRenderer : asset.meshRenderers) {
    auto entity = getOrCreateEntity(pRenderer.entity);
    mEntityDatabase.set(entity, pRenderer.value);
  }

  for (const auto &pRenderer : asset.skinnedMeshRenderers) {
    auto entity = getOrCreateEntity(pRenderer.entity);
    mEntityDatabase.set(entity, pRenderer.value);
  }

  for (const auto &pSkeleton : asset.skeletons) {
    auto entity = getOrCreateEntity(pSkeleton.entity);
    const auto &asset = pSkeleton.value.get();

    usize numJoints = asset.jointLocalPositions.size();

    mEntityDatabase.set(entity, SkeletonAssetRef{pSkeleton.value});
  }

  for (auto &item : asset.animators) {
    auto entity = getOrCreateEntity(item.entity);

    AnimatorAssetRef animator{item.value};
    mEntityDatabase.set(entity, animator);
  }

  for (auto &item : asset.directionalLights) {
    auto entity = getOrCreateEntity(item.entity);
    mEntityDatabase.set<DirectionalLight>(entity, item.value);
  }

  for (auto &item : asset.pointLights) {
    auto entity = getOrCreateEntity(item.entity);
    mEntityDatabase.set<PointLight>(entity, item.value);
  }

  std::vector<Entity> rootEntities;
  for (auto entity : entities) {
    if (!mEntityDatabase.has<Parent>(entity)) {
      rootEntities.push_back(entity);
    }
  }

  QuollAssert(!rootEntities.empty(),
              "Nothing is spawned. Check that prefab is not empty.");

  auto rootNode = Entity::Null;
  // If more than one root exists,
  // create root node
  if (rootEntities.size() > 1) {
    rootNode = mEntityDatabase.create();
    mEntityDatabase.set<Children>(rootNode, {rootEntities});
    for (auto entity : rootEntities) {
      mEntityDatabase.set<Parent>(entity, {rootNode});
    }
    entities.push_back(rootNode);
    mEntityDatabase.set<Name>(rootNode, {assetName});
  } else {
    rootNode = rootEntities.at(0);
  }

  mEntityDatabase.set(rootNode, transform);
  mEntityDatabase.set<WorldTransform>(rootNode, {});

  return entities;
}

Entity EntitySpawner::spawnSprite(AssetRef<TextureAsset> texture,
                                  LocalTransform transform) {
  auto entity = mEntityDatabase.create();

  auto name = texture.meta().name;

  mEntityDatabase.set(entity, transform);
  mEntityDatabase.set<WorldTransform>(entity, {});
  mEntityDatabase.set(entity, Name{name});
  mEntityDatabase.set<Sprite>(entity, {texture});

  return entity;
}

} // namespace quoll
