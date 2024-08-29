#include "quoll/core/Base.h"
#include "quoll/renderer/Mesh.h"
#include "EntityMeshActions.h"

namespace quoll::editor {

EntitySetMesh::EntitySetMesh(Entity entity, AssetHandle<MeshAsset> mesh)
    : mEntity(entity), mMesh(mesh) {}

ActionExecutorResult EntitySetMesh::onExecute(WorkspaceState &state,
                                              AssetRegistry &assetRegistry) {
  auto &scene = state.scene;

  auto &db = scene.entityDatabase;

  if (db.has<Mesh>(mEntity)) {
    mOldMesh = db.get<Mesh>(mEntity).handle;
  }

  db.set<Mesh>(mEntity, {mMesh});

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
    db.set<Mesh>(mEntity, {mOldMesh});

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

  db.set<Mesh>(mEntity, {mOldMesh});

  ActionExecutorResult res;
  res.entitiesToSave.push_back(mEntity);

  return res;
}

bool EntityDeleteMesh::predicate(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) {
  return state.scene.entityDatabase.has<quoll::Mesh>(mEntity);
}

} // namespace quoll::editor
