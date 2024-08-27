#include "quoll/core/Base.h"
#include "quoll/renderer/Mesh.h"
#include "EntityMeshActions.h"

namespace quoll::editor {

static void replaceMesh(AssetType type, AssetHandle<MeshAsset> mesh,
                        Entity entity, EntityDatabase &db) {
  if (type == AssetType::Mesh) {
    db.set<Mesh>(entity, {mesh});
  }
}

EntitySetMesh::EntitySetMesh(Entity entity, AssetHandle<MeshAsset> mesh)
    : mEntity(entity), mMesh(mesh) {}

ActionExecutorResult EntitySetMesh::onExecute(WorkspaceState &state,
                                              AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  auto &db = scene.entityDatabase;

  if (db.has<Mesh>(mEntity)) {
    mOldMesh = db.get<Mesh>(mEntity).handle;
  }

  auto type = assetRegistry.getMeta(mMesh).type;
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

  if (mOldMesh) {
    auto type = assetRegistry.getMeta(mOldMesh).type;
    replaceMesh(type, mOldMesh, mEntity, db);
  } else {
    if (db.has<Mesh>(mEntity)) {
      db.remove<Mesh>(mEntity);
    }
  }

  ActionExecutorResult res;
  res.entitiesToSave.push_back(mEntity);

  return res;
}

bool EntitySetMesh::predicate(WorkspaceState &state,
                              AssetRegistry &assetRegistry) {
  return assetRegistry.has(mMesh);
}

EntityDeleteMesh::EntityDeleteMesh(Entity entity) : mEntity(entity) {}

ActionExecutorResult EntityDeleteMesh::onExecute(WorkspaceState &state,
                                                 AssetRegistry &assetRegistry) {
  auto &scene = state.scene;
  auto &db = scene.entityDatabase;

  if (db.has<Mesh>(mEntity)) {
    mOldMesh = db.get<Mesh>(mEntity).handle;
    db.remove<Mesh>(mEntity);
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
  auto type = assetRegistry.getMeta(mOldMesh).type;

  replaceMesh(type, mOldMesh, mEntity, db);

  ActionExecutorResult res;
  res.entitiesToSave.push_back(mEntity);

  return res;
}

bool EntityDeleteMesh::predicate(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  return scene.entityDatabase.has<quoll::Mesh>(mEntity);
}

} // namespace quoll::editor
