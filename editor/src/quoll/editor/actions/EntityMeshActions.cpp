#include "quoll/core/Base.h"
#include "EntityMeshActions.h"

namespace quoll::editor {

/**
 * @brief Replace mesh with new one
 *
 * @param type Mesh asset type
 * @param mesh Mesh asset handle
 * @param entity Entity
 * @param db Entity database
 */
static void replaceMesh(AssetType type, MeshAssetHandle mesh, Entity entity,
                        EntityDatabase &db) {
  if (type == AssetType::Mesh) {
    db.set<Mesh>(entity, {mesh});

    if (db.has<SkinnedMesh>(entity)) {
      db.remove<SkinnedMesh>(entity);
    }
  } else if (type == AssetType::SkinnedMesh) {
    db.set<SkinnedMesh>(entity, {mesh});

    if (db.has<Mesh>(entity)) {
      db.remove<Mesh>(entity);
    }
  }
}

EntitySetMesh::EntitySetMesh(Entity entity, MeshAssetHandle mesh)
    : mEntity(entity), mMesh(mesh) {}

ActionExecutorResult EntitySetMesh::onExecute(WorkspaceState &state,
                                              AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  auto &db = scene.entityDatabase;

  if (db.has<Mesh>(mEntity)) {
    mOldMesh = db.get<Mesh>(mEntity).handle;
  } else if (db.has<SkinnedMesh>(mEntity)) {
    mOldMesh = db.get<SkinnedMesh>(mEntity).handle;
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
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  auto &db = scene.entityDatabase;

  if (mOldMesh != MeshAssetHandle::Null) {
    auto type = assetRegistry.getMeshes().getAsset(mOldMesh).type;
    replaceMesh(type, mOldMesh, mEntity, db);
  } else {
    if (db.has<Mesh>(mEntity)) {
      db.remove<Mesh>(mEntity);
    } else if (db.has<SkinnedMesh>(mEntity)) {
      db.remove<SkinnedMesh>(mEntity);
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
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  auto &db = scene.entityDatabase;

  if (db.has<Mesh>(mEntity)) {
    mOldMesh = db.get<Mesh>(mEntity).handle;
    db.remove<Mesh>(mEntity);
  } else if (db.has<SkinnedMesh>(mEntity)) {
    mOldMesh = db.get<SkinnedMesh>(mEntity).handle;
    db.remove<SkinnedMesh>(mEntity);
  }

  ActionExecutorResult res;
  res.entitiesToSave.push_back(mEntity);
  res.addToHistory = true;

  return res;
}

ActionExecutorResult EntityDeleteMesh::onUndo(WorkspaceState &state,
                                              AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;
  auto &db = scene.entityDatabase;
  auto type = assetRegistry.getMeshes().getAsset(mOldMesh).type;

  replaceMesh(type, mOldMesh, mEntity, db);

  ActionExecutorResult res;
  res.entitiesToSave.push_back(mEntity);

  return res;
}

bool EntityDeleteMesh::predicate(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) {
  auto &scene = state.mode == WorkspaceMode::Simulation ? state.simulationScene
                                                        : state.scene;

  return scene.entityDatabase.has<quoll::Mesh>(mEntity) ||
         scene.entityDatabase.has<quoll::SkinnedMesh>(mEntity);
}

} // namespace quoll::editor
