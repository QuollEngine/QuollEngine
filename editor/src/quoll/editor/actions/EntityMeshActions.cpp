#include "quoll/core/Base.h"
#include "quoll/renderer/Mesh.h"
#include "quoll/renderer/SkinnedMesh.h"
#include "EntityMeshActions.h"

namespace quoll::editor {

static void replaceMesh(AssetType type, MeshAssetHandle mesh, Entity entity,
                        EntityDatabase &db) {
  if (type == AssetType::Mesh) {
    entity.set<Mesh>({mesh});

    if (entity.has<SkinnedMesh>()) {
      entity.remove<SkinnedMesh>();
    }
  } else if (type == AssetType::SkinnedMesh) {
    entity.set<SkinnedMesh>({mesh});

    if (entity.has<Mesh>()) {
      entity.remove<Mesh>();
    }
  }
}

EntitySetMesh::EntitySetMesh(Entity entity, MeshAssetHandle mesh)
    : mEntity(entity), mMesh(mesh) {}

ActionExecutorResult EntitySetMesh::onExecute(WorkspaceState &state,
                                              AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  auto &db = scene.entityDatabase;

  if (mEntity.has<Mesh>()) {
    mOldMesh = mEntity.get_ref<Mesh>()->handle;
  } else if (mEntity.has<SkinnedMesh>()) {
    mOldMesh = mEntity.get_ref<SkinnedMesh>()->handle;
  }

  auto type = assetRegistry.getMeshes().getAsset(mMesh).type;
  replaceMesh(type, mMesh, mEntity, db);

  ActionExecutorResult res;
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;

  return res;
}

ActionExecutorResult EntitySetMesh::onUndo(WorkspaceState &state,
                                           AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  auto &db = scene.entityDatabase;

  if (mOldMesh != MeshAssetHandle::Null) {
    auto type = assetRegistry.getMeshes().getAsset(mOldMesh).type;
    replaceMesh(type, mOldMesh, mEntity, db);
  } else {
    if (mEntity.has<Mesh>()) {
      mEntity.remove<Mesh>();
    } else if (mEntity.has<SkinnedMesh>()) {
      mEntity.remove<SkinnedMesh>();
    }
  }

  ActionExecutorResult res;
  res.entitiesToSave.push_back(mEntity);

  return res;
}

bool EntitySetMesh::predicate(WorkspaceState &state,
                              AssetRegistry &assetRegistry) {
  return assetRegistry.getMeshes().hasAsset(mMesh);
}

EntityDeleteMesh::EntityDeleteMesh(Entity entity) : mEntity(entity) {}

ActionExecutorResult EntityDeleteMesh::onExecute(WorkspaceState &state,
                                                 AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  auto &db = scene.entityDatabase;

  if (mEntity.has<Mesh>()) {
    mOldMesh = mEntity.get_ref<Mesh>()->handle;
    mEntity.remove<Mesh>();
  } else if (mEntity.has<SkinnedMesh>()) {
    mOldMesh = mEntity.get_ref<SkinnedMesh>()->handle;
    mEntity.remove<SkinnedMesh>();
  }

  ActionExecutorResult res;
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;

  return res;
}

ActionExecutorResult EntityDeleteMesh::onUndo(WorkspaceState &state,
                                              AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  auto &db = scene.entityDatabase;
  auto type = assetRegistry.getMeshes().getAsset(mOldMesh).type;

  replaceMesh(type, mOldMesh, mEntity, db);

  ActionExecutorResult res;
  res.entitiesToSave.push_back(mEntity);

  return res;
}

bool EntityDeleteMesh::predicate(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) {
  return mEntity.has<quoll::Mesh>() || mEntity.has<quoll::SkinnedMesh>();
}

} // namespace quoll::editor
