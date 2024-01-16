#include "quoll/core/Base.h"
#include "quoll/renderer/Mesh.h"
#include "quoll/renderer/SkinnedMesh.h"

#include "quoll/editor/actions/EntityMeshActions.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

class EntityMeshActionTestBase : public ActionTestBase {
public:
  quoll::MeshAssetHandle createMesh() {
    quoll::AssetData<quoll::MeshAsset> asset{};
    asset.type = quoll::AssetType::Mesh;
    return assetRegistry.getMeshes().addAsset(asset);
  }

  quoll::MeshAssetHandle createSkinnedMesh() {
    quoll::AssetData<quoll::MeshAsset> asset{};
    asset.type = quoll::AssetType::SkinnedMesh;
    return assetRegistry.getMeshes().addAsset(asset);
  }
};

using EntitySetMeshTest = EntityMeshActionTestBase;

TEST_F(EntitySetMeshTest, ExecutorSetsMeshComponentIfMeshTypeIsMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto mesh = createMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Mesh>(entity).handle, mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshTest,
       ExecutorSetsSkinnedMeshComponentIfMeshTypeIsSkinnedMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto mesh = createSkinnedMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::SkinnedMesh>(entity).handle,
            mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshTest,
       ExecutorReplacesSkinnedMeshWithMeshIfNewAssetTypeIsMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createMesh();
  state.scene.entityDatabase.set<quoll::SkinnedMesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Mesh>(entity).handle, mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshTest,
       ExecutorReplacesMeshWithSkinnedMeshIfNewAssetTypeIsSkinnedMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createMesh();
  auto mesh = createSkinnedMesh();
  state.scene.entityDatabase.set<quoll::Mesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::SkinnedMesh>(entity).handle,
            mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshTest, UndoSetsPreviousMeshHandleIfSameComponent) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createMesh();
  auto mesh = createMesh();

  state.scene.entityDatabase.set<quoll::Mesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Mesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, UndoSetsPreviousSkinnedMeshHandleIfSameComponent) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createSkinnedMesh();

  state.scene.entityDatabase.set<quoll::SkinnedMesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::SkinnedMesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, UndoReplacesNewMeshWithOldSkinnedMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createMesh();

  state.scene.entityDatabase.set<quoll::SkinnedMesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::SkinnedMesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, UndoReplacesNewSkinnedMeshWithOldMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createMesh();
  auto mesh = createSkinnedMesh();

  state.scene.entityDatabase.set<quoll::Mesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Mesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, UndoDeletesMeshComponentIfNoPreviousComponent) {
  auto entity = state.scene.entityDatabase.create();
  auto mesh = createMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest,
       UndoDeletesSkinnedMeshComponentIfNoPreviousComponent) {
  auto entity = state.scene.entityDatabase.create();
  auto mesh = createSkinnedMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, PredicateReturnsTrueIfMeshExists) {
  auto entity = state.scene.entityDatabase.create();
  auto mesh = createSkinnedMesh();

  EXPECT_TRUE(quoll::editor::EntitySetMesh(entity, mesh)
                  .predicate(state, assetRegistry));
}

TEST_F(EntitySetMeshTest, PredicateReturnsFalseIfMeshDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  EXPECT_FALSE(quoll::editor::EntitySetMesh(entity, quoll::MeshAssetHandle{25})
                   .predicate(state, assetRegistry));
}

using EntityDeleteMeshTest = EntityMeshActionTestBase;

TEST_F(EntityDeleteMeshTest, ExecutorDeletesMeshIfCurrentComponentIsMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createMesh();

  state.scene.entityDatabase.set<quoll::Mesh>(entity, {oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityDeleteMeshTest,
       ExecutorDeletesSkinnedMeshIfCurrentComponentIsSkinnedMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createSkinnedMesh();

  state.scene.entityDatabase.set<quoll::SkinnedMesh>(entity, {oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityDeleteMeshTest, UndoRecreatesMeshIfDeletedComponentWasMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createMesh();

  state.scene.entityDatabase.set<quoll::Mesh>(entity, {oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Mesh>(entity).handle,
            oldMesh);
}

TEST_F(EntityDeleteMeshTest,
       UndoRecreatesSkinnedMeshIfDeletedComponentWasSkinnedMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createSkinnedMesh();

  state.scene.entityDatabase.set<quoll::SkinnedMesh>(entity, {oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::SkinnedMesh>(entity).handle,
            oldMesh);
}

TEST_F(EntityDeleteMeshTest, PredicateReturnsTrueIfEntityHasMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto mesh = createMesh();
  state.scene.entityDatabase.set<quoll::Mesh>(entity, {mesh});

  EXPECT_TRUE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}

TEST_F(EntityDeleteMeshTest, PredicateReturnsTrueIfEntityHasSkinnedMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto mesh = createSkinnedMesh();
  state.scene.entityDatabase.set<quoll::SkinnedMesh>(entity, {mesh});

  EXPECT_TRUE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}

TEST_F(EntityDeleteMeshTest,
       PredicateReturnsFalseIfEntityHasNoMeshOrSkinnedMesh) {
  auto entity = state.scene.entityDatabase.create();

  EXPECT_FALSE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}
