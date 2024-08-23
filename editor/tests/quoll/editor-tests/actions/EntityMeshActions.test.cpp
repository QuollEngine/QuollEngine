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
  auto entity = state.scene.entityDatabase.entity();
  auto mesh = createMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  ASSERT_TRUE(entity.has<quoll::Mesh>());
  EXPECT_EQ(entity.get_ref<quoll::Mesh>()->handle, mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshTest,
       ExecutorSetsSkinnedMeshComponentIfMeshTypeIsSkinnedMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto mesh = createSkinnedMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  ASSERT_TRUE(entity.has<quoll::SkinnedMesh>());
  EXPECT_EQ(entity.get_ref<quoll::SkinnedMesh>()->handle, mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshTest,
       ExecutorReplacesSkinnedMeshWithMeshIfNewAssetTypeIsMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createMesh();
  entity.set<quoll::SkinnedMesh>({oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::SkinnedMesh>());
  ASSERT_TRUE(entity.has<quoll::Mesh>());
  EXPECT_EQ(entity.get_ref<quoll::Mesh>()->handle, mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshTest,
       ExecutorReplacesMeshWithSkinnedMeshIfNewAssetTypeIsSkinnedMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto oldMesh = createMesh();
  auto mesh = createSkinnedMesh();
  entity.set<quoll::Mesh>({oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::Mesh>());
  ASSERT_TRUE(entity.has<quoll::SkinnedMesh>());
  EXPECT_EQ(entity.get_ref<quoll::SkinnedMesh>()->handle, mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshTest, UndoSetsPreviousMeshHandleIfSameComponent) {
  auto entity = state.scene.entityDatabase.entity();
  auto oldMesh = createMesh();
  auto mesh = createMesh();

  entity.set<quoll::Mesh>({oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  ASSERT_TRUE(entity.has<quoll::Mesh>());
  EXPECT_EQ(entity.get_ref<quoll::Mesh>()->handle, oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, UndoSetsPreviousSkinnedMeshHandleIfSameComponent) {
  auto entity = state.scene.entityDatabase.entity();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createSkinnedMesh();

  entity.set<quoll::SkinnedMesh>({oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  ASSERT_TRUE(entity.has<quoll::SkinnedMesh>());
  EXPECT_EQ(entity.get_ref<quoll::SkinnedMesh>()->handle, oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, UndoReplacesNewMeshWithOldSkinnedMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createMesh();

  entity.set<quoll::SkinnedMesh>({oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::Mesh>());
  ASSERT_TRUE(entity.has<quoll::SkinnedMesh>());
  EXPECT_EQ(entity.get_ref<quoll::SkinnedMesh>()->handle, oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, UndoReplacesNewSkinnedMeshWithOldMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto oldMesh = createMesh();
  auto mesh = createSkinnedMesh();

  entity.set<quoll::Mesh>({oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::SkinnedMesh>());
  ASSERT_TRUE(entity.has<quoll::Mesh>());
  EXPECT_EQ(entity.get_ref<quoll::Mesh>()->handle, oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, UndoDeletesMeshComponentIfNoPreviousComponent) {
  auto entity = state.scene.entityDatabase.entity();
  auto mesh = createMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::Mesh>());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest,
       UndoDeletesSkinnedMeshComponentIfNoPreviousComponent) {
  auto entity = state.scene.entityDatabase.entity();
  auto mesh = createSkinnedMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::SkinnedMesh>());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, PredicateReturnsTrueIfMeshExists) {
  auto entity = state.scene.entityDatabase.entity();
  auto mesh = createSkinnedMesh();

  EXPECT_TRUE(quoll::editor::EntitySetMesh(entity, mesh)
                  .predicate(state, assetRegistry));
}

TEST_F(EntitySetMeshTest, PredicateReturnsFalseIfMeshDoesNotExist) {
  auto entity = state.scene.entityDatabase.entity();

  EXPECT_FALSE(quoll::editor::EntitySetMesh(entity, quoll::MeshAssetHandle{25})
                   .predicate(state, assetRegistry));
}

using EntityDeleteMeshTest = EntityMeshActionTestBase;

TEST_F(EntityDeleteMeshTest, ExecutorDeletesMeshIfCurrentComponentIsMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto oldMesh = createMesh();

  entity.set<quoll::Mesh>({oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::Mesh>());
  EXPECT_FALSE(entity.has<quoll::SkinnedMesh>());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityDeleteMeshTest,
       ExecutorDeletesSkinnedMeshIfCurrentComponentIsSkinnedMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto oldMesh = createSkinnedMesh();

  entity.set<quoll::SkinnedMesh>({oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(entity.has<quoll::Mesh>());
  EXPECT_FALSE(entity.has<quoll::SkinnedMesh>());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityDeleteMeshTest, UndoRecreatesMeshIfDeletedComponentWasMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto oldMesh = createMesh();

  entity.set<quoll::Mesh>({oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  ASSERT_TRUE(entity.has<quoll::Mesh>());
  EXPECT_EQ(entity.get_ref<quoll::Mesh>()->handle, oldMesh);
}

TEST_F(EntityDeleteMeshTest,
       UndoRecreatesSkinnedMeshIfDeletedComponentWasSkinnedMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto oldMesh = createSkinnedMesh();

  entity.set<quoll::SkinnedMesh>({oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(entity.has<quoll::SkinnedMesh>());
  EXPECT_EQ(entity.get_ref<quoll::SkinnedMesh>()->handle, oldMesh);
}

TEST_F(EntityDeleteMeshTest, PredicateReturnsTrueIfEntityHasMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto mesh = createMesh();
  entity.set<quoll::Mesh>({mesh});

  EXPECT_TRUE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}

TEST_F(EntityDeleteMeshTest, PredicateReturnsTrueIfEntityHasSkinnedMesh) {
  auto entity = state.scene.entityDatabase.entity();
  auto mesh = createSkinnedMesh();
  entity.set<quoll::SkinnedMesh>({mesh});

  EXPECT_TRUE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}

TEST_F(EntityDeleteMeshTest,
       PredicateReturnsFalseIfEntityHasNoMeshOrSkinnedMesh) {
  auto entity = state.scene.entityDatabase.entity();

  EXPECT_FALSE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}
