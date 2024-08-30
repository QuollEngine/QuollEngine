#include "quoll/core/Base.h"
#include "quoll/renderer/Mesh.h"
#include "quoll/editor/actions/EntityMeshActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

class EntityMeshActionTestBase : public ActionTestBase {
public:
  quoll::AssetHandle<quoll::MeshAsset> createMesh() {
    quoll::AssetData<quoll::MeshAsset> asset{};
    asset.type = quoll::AssetType::Mesh;
    return assetCache.getRegistry().add(asset);
  }
};

using EntitySetMeshTest = EntityMeshActionTestBase;

TEST_F(EntitySetMeshTest, ExecutorSetsMeshComponentIfMeshTypeIsMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto mesh = createMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetCache);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Mesh>(entity).handle, mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshTest,
       UndoSetsPreviousMeshHandleIfHadPreviousMeshComponent) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createMesh();
  auto mesh = createMesh();

  state.scene.entityDatabase.set<quoll::Mesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_TRUE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(state.scene.entityDatabase.get<quoll::Mesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, UndoDeletesMeshComponentIfNoPreviousComponent) {
  auto entity = state.scene.entityDatabase.create();
  auto mesh = createMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetCache);
  auto res = action.onUndo(state, assetCache);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_F(EntitySetMeshTest, PredicateReturnsFalseIfMeshDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  EXPECT_FALSE(quoll::editor::EntitySetMesh(
                   entity, quoll::AssetHandle<quoll::MeshAsset>{25})
                   .predicate(state, assetCache));
}

using EntityDeleteMeshTest = EntityMeshActionTestBase;

TEST_F(EntityDeleteMeshTest, ExecutorDeletesMeshIfCurrentComponentIsMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto oldMesh = createMesh();

  state.scene.entityDatabase.set<quoll::Mesh>(entity, {oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  auto res = action.onExecute(state, assetCache);

  EXPECT_FALSE(state.scene.entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityDeleteMeshTest, PredicateReturnsTrueIfEntityHasMesh) {
  auto entity = state.scene.entityDatabase.create();
  auto mesh = createMesh();
  state.scene.entityDatabase.set<quoll::Mesh>(entity, {mesh});

  EXPECT_TRUE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetCache));
}

TEST_F(EntityDeleteMeshTest, PredicateReturnsFalseIfEntityHasNoMesh) {
  auto entity = state.scene.entityDatabase.create();

  EXPECT_FALSE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetCache));
}
