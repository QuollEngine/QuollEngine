#include "quoll/core/Base.h"
#include "quoll/core/Delete.h"
#include "quoll/core/Name.h"
#include "quoll/entity/EntitySpawner.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Parent.h"
#include "EntityLightActions.h"
#include "EntitySkeletonActions.h"
#include "EntityTransformActions.h"
#include "EntityUpdateComponentAction.h"
#include "SpawnEntityActions.h"

namespace quoll::editor {

static LocalTransform getTransformFromView(glm::mat4 viewMatrix) {
  static constexpr glm::vec3 DistanceFromEye{0.0f, 0.0f, -10.0f};
  const auto &invViewMatrix = glm::inverse(viewMatrix);
  const auto &orientation = invViewMatrix * glm::translate(DistanceFromEye);

  LocalTransform transform{};
  transform.localPosition = orientation[3];

  return transform;
}

static bool isPrefabValid(const PrefabAsset &prefab) {
  return !prefab.animators.empty() || !prefab.meshes.empty() ||
         !prefab.skeletons.empty() || !prefab.transforms.empty() ||
         !prefab.directionalLights.empty() || !prefab.pointLights.empty() ||
         !prefab.meshRenderers.empty() || !prefab.skinnedMeshRenderers.empty();
}

ActionExecutorResult SpawnEmptyEntityAtView::onExecute(WorkspaceState &state,
                                                       AssetCache &assetCache) {
  auto &scene = state.scene;
  const auto &viewMatrix =
      scene.entityDatabase.get<Camera>(state.camera).viewMatrix;

  auto transform = getTransformFromView(viewMatrix);

  mSpawnedEntity = EntitySpawner(scene.entityDatabase, assetCache.getRegistry())
                       .spawnEmpty(transform);

  EntityUpdateComponent<Name>(mSpawnedEntity, {}, Name{"New entity"})
      .onExecute(state, assetCache);

  ActionExecutorResult res{};
  res.entitiesToSave.push_back(mSpawnedEntity);
  res.addToHistory = true;

  return res;
}

ActionExecutorResult SpawnEmptyEntityAtView::onUndo(WorkspaceState &state,
                                                    AssetCache &assetCache) {
  auto &scene = state.scene;

  bool preserveSelectedEntity = true;
  {
    auto current = state.selectedEntity;

    preserveSelectedEntity = current != mSpawnedEntity;
    while (preserveSelectedEntity &&
           scene.entityDatabase.has<Parent>(current)) {
      auto parent = scene.entityDatabase.get<Parent>(current).parent;
      preserveSelectedEntity = parent != mSpawnedEntity;
      current = parent;
    }
  }

  if (!preserveSelectedEntity) {
    state.selectedEntity = Entity::Null;
  }

  scene.entityDatabase.set<Delete>(mSpawnedEntity, {});
  ActionExecutorResult res{};
  res.entitiesToDelete.push_back(mSpawnedEntity);
  return res;
}

bool SpawnEmptyEntityAtView::predicate(WorkspaceState &state,
                                       AssetCache &assetCache) {
  auto &scene = state.scene;

  return scene.entityDatabase.has<Camera>(state.camera);
}

SpawnPrefabAtView::SpawnPrefabAtView(AssetRef<PrefabAsset> prefab,
                                     Entity camera)
    : mPrefab(prefab), mCamera(camera) {}

ActionExecutorResult SpawnPrefabAtView::onExecute(WorkspaceState &state,
                                                  AssetCache &assetCache) {
  auto &scene = state.scene;

  const auto &viewMatrix = scene.entityDatabase.get<Camera>(mCamera).viewMatrix;

  ActionExecutorResult res{};
  res.entitiesToSave =
      EntitySpawner(scene.entityDatabase, assetCache.getRegistry())
          .spawnPrefab(mPrefab.handle(), getTransformFromView(viewMatrix));
  res.addToHistory = true;

  mSpawnedRootEntity = res.entitiesToSave.back();

  return res;
}

ActionExecutorResult SpawnPrefabAtView::onUndo(WorkspaceState &state,
                                               AssetCache &assetCache) {
  auto &scene = state.scene;

  bool preserveSelectedEntity = true;
  {
    auto current = state.selectedEntity;

    preserveSelectedEntity = current != mSpawnedRootEntity;
    while (preserveSelectedEntity &&
           scene.entityDatabase.has<Parent>(current)) {
      auto parent = scene.entityDatabase.get<Parent>(current).parent;
      preserveSelectedEntity = parent != mSpawnedRootEntity;
      current = parent;
    }
  }

  if (!preserveSelectedEntity) {
    state.selectedEntity = Entity::Null;
  }

  scene.entityDatabase.set<Delete>(mSpawnedRootEntity, {});
  ActionExecutorResult res{};
  res.entitiesToDelete.push_back(mSpawnedRootEntity);
  return res;
}

bool SpawnPrefabAtView::predicate(WorkspaceState &state,
                                  AssetCache &assetCache) {
  auto &scene = state.scene;

  return mPrefab && isPrefabValid(mPrefab.get()) &&
         scene.entityDatabase.has<Camera>(mCamera);
}

SpawnSpriteAtView::SpawnSpriteAtView(AssetRef<TextureAsset> texture,
                                     Entity camera)
    : mTexture(texture), mCamera(camera) {}

ActionExecutorResult SpawnSpriteAtView::onExecute(WorkspaceState &state,
                                                  AssetCache &assetCache) {
  auto &scene = state.scene;

  const auto &viewMatrix = scene.entityDatabase.get<Camera>(mCamera).viewMatrix;

  ActionExecutorResult res{};
  mSpawnedEntity =
      EntitySpawner(scene.entityDatabase, assetCache.getRegistry())
          .spawnSprite(mTexture.handle(), getTransformFromView(viewMatrix));

  res.entitiesToSave.push_back(mSpawnedEntity);
  res.addToHistory = true;

  return res;
}

ActionExecutorResult SpawnSpriteAtView::onUndo(WorkspaceState &state,
                                               AssetCache &assetCache) {
  auto &scene = state.scene;

  bool preserveSelectedEntity = true;
  {
    auto current = state.selectedEntity;

    preserveSelectedEntity = current != mSpawnedEntity;
    while (preserveSelectedEntity &&
           scene.entityDatabase.has<Parent>(current)) {
      auto parent = scene.entityDatabase.get<Parent>(current).parent;
      preserveSelectedEntity = parent != mSpawnedEntity;
      current = parent;
    }
  }

  if (!preserveSelectedEntity) {
    state.selectedEntity = Entity::Null;
  }

  scene.entityDatabase.set<Delete>(mSpawnedEntity, {});
  ActionExecutorResult res{};
  res.entitiesToDelete.push_back(mSpawnedEntity);
  return res;
}

bool SpawnSpriteAtView::predicate(WorkspaceState &state,
                                  AssetCache &assetCache) {
  auto &scene = state.scene;

  return mTexture && scene.entityDatabase.has<Camera>(mCamera);
}

} // namespace quoll::editor
