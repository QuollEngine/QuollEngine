#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/core/Id.h"
#include "quoll/core/Name.h"
#include "quoll/animation/Animator.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/audio/AudioSource.h"
#include "quoll/audio/AudioStart.h"
#include "quoll/audio/AudioStatus.h"
#include "quoll/input/InputMap.h"
#include "quoll/lua-scripting/LuaScript.h"
#include "quoll/physics/Collidable.h"
#include "quoll/physics/Force.h"
#include "quoll/physics/RigidBody.h"
#include "quoll/physics/RigidBodyClear.h"
#include "quoll/physics/Torque.h"
#include "quoll/physx/PhysxInstance.h"
#include "quoll/renderer/Mesh.h"
#include "quoll/renderer/MeshRenderer.h"
#include "quoll/renderer/SkinnedMesh.h"
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
                             AssetRegistry &assetRegistry)
    : mEntityDatabase(entityDatabase), mAssetRegistry(assetRegistry) {}

Entity EntitySpawner::spawnEmpty(LocalTransform transform) {
  auto entity = mEntityDatabase.entity();
  entity.set(transform);
  entity.set<WorldTransform>({});
  entity.set<Name>({"New entity"});
  return entity;
}

std::vector<Entity> EntitySpawner::spawnPrefab(PrefabAssetHandle handle,
                                               LocalTransform transform) {
  QuollAssert(mAssetRegistry.getPrefabs().hasAsset(handle), "Prefab not found");

  const auto &assetName = mAssetRegistry.getPrefabs().getAsset(handle).name;
  const auto &asset = mAssetRegistry.getPrefabs().getAsset(handle).data;

  std::unordered_map<u32, usize> entityMap;
  std::vector<Entity> entities;

  auto getOrCreateEntity = [&entityMap, &entities, this](u32 localId) mutable {
    if (entityMap.find(localId) == entityMap.end()) {
      auto entity = mEntityDatabase.entity();
      entity.set<LocalTransform>({});
      entity.set<WorldTransform>({});
      entity.set<Name>({"New entity"});

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
      entity.set(Parent{parent});

      if (!parent.has<Children>()) {
        parent.set<Children>({{entity}});
      } else {
        parent.get_ref<Children>()->children.push_back(entity);
      }
    }
  }

  for (const auto &pTransform : asset.transforms) {
    auto entity = getOrCreateEntity(pTransform.entity);
    LocalTransform transform{};
    transform.localPosition = pTransform.value.position;
    transform.localRotation = pTransform.value.rotation;
    transform.localScale = pTransform.value.scale;

    entity.set<LocalTransform>(transform);
  }

  for (const auto &pName : asset.names) {
    auto entity = getOrCreateEntity(pName.entity);
    Name name{};
    name.name = pName.value;
    entity.set<Name>(name);
  }

  for (const auto &pMesh : asset.meshes) {
    auto entity = getOrCreateEntity(pMesh.entity);

    auto type = mAssetRegistry.getMeshes().getAsset(pMesh.value).type;
    if (type == AssetType::Mesh) {
      entity.set<Mesh>({pMesh.value});
    } else if (type == AssetType::SkinnedMesh) {
      entity.set<SkinnedMesh>({pMesh.value});
    }
  }

  for (const auto &pRenderer : asset.meshRenderers) {
    auto entity = getOrCreateEntity(pRenderer.entity);
    entity.set(pRenderer.value);
  }

  for (const auto &pRenderer : asset.skinnedMeshRenderers) {
    auto entity = getOrCreateEntity(pRenderer.entity);
    entity.set(pRenderer.value);
  }

  for (const auto &pSkeleton : asset.skeletons) {
    auto entity = getOrCreateEntity(pSkeleton.entity);

    const auto &asset =
        mAssetRegistry.getSkeletons().getAsset(pSkeleton.value).data;

    usize numJoints = asset.jointLocalPositions.size();

    Skeleton skeleton{};
    skeleton.assetHandle = pSkeleton.value;
    skeleton.numJoints = static_cast<u32>(numJoints);
    skeleton.jointNames.resize(numJoints);
    skeleton.jointParents.resize(numJoints);
    skeleton.jointLocalPositions.resize(numJoints);
    skeleton.jointLocalRotations.resize(numJoints);
    skeleton.jointLocalScales.resize(numJoints);
    skeleton.jointInverseBindMatrices.resize(numJoints);
    skeleton.jointWorldTransforms.resize(numJoints, glm::mat4{1.0f});
    skeleton.jointFinalTransforms.resize(numJoints, glm::mat4{1.0f});

    for (usize i = 0; i < numJoints; ++i) {
      skeleton.jointNames.at(i) = asset.jointNames.at(i);
      skeleton.jointParents.at(i) = asset.jointParents.at(i);
      skeleton.jointLocalPositions.at(i) = asset.jointLocalPositions.at(i);
      skeleton.jointLocalRotations.at(i) = asset.jointLocalRotations.at(i);
      skeleton.jointLocalScales.at(i) = asset.jointLocalScales.at(i);
      skeleton.jointInverseBindMatrices.at(i) =
          asset.jointInverseBindMatrices.at(i);
    }

    entity.set(skeleton);
  }

  for (auto &item : asset.animators) {
    auto entity = getOrCreateEntity(item.entity);
    const auto &asset = mAssetRegistry.getAnimators().getAsset(item.value);

    Animator animator{};
    animator.asset = item.value;
    animator.currentState = asset.data.initialState;
    entity.set(animator);
  }

  for (auto &item : asset.directionalLights) {
    auto entity = getOrCreateEntity(item.entity);
    entity.set<DirectionalLight>(item.value);
  }

  for (auto &item : asset.pointLights) {
    auto entity = getOrCreateEntity(item.entity);
    entity.set<PointLight>(item.value);
  }

  std::vector<Entity> rootEntities;
  for (auto entity : entities) {
    if (!entity.has<Parent>()) {
      rootEntities.push_back(entity);
    }
  }

  QuollAssert(!rootEntities.empty(),
              "Nothing is spawned. Check that prefab is not empty.");

  flecs::entity rootNode;
  // If more than one root exists,
  // create root node
  if (rootEntities.size() > 1) {
    rootNode = mEntityDatabase.entity();
    rootNode.set<Children>({rootEntities});
    for (auto entity : rootEntities) {
      entity.set<Parent>({rootNode});
    }
    entities.push_back(rootNode);
    rootNode.set<Name>({assetName});
  } else {
    rootNode = rootEntities.at(0);
  }

  rootNode.set(transform);
  rootNode.set<WorldTransform>({});

  return entities;
}

Entity EntitySpawner::spawnSprite(TextureAssetHandle handle,
                                  LocalTransform transform) {
  auto entity = mEntityDatabase.entity();

  auto name = mAssetRegistry.getTextures().getAsset(handle).name;

  entity.set(transform);
  entity.set<WorldTransform>({});
  entity.set(Name{name});
  entity.set<Sprite>({handle});

  return entity;
}

} // namespace quoll
