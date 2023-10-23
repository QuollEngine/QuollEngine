#include "quoll/core/Base.h"
#include "quoll/scene/Mesh.h"
#include "quoll/scene/SkinnedMesh.h"

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

TEST_P(EntitySetMeshTest, ExecutorSetsMeshComponentIfMeshTypeIsMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Mesh>(entity).handle, mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetMeshTest,
       ExecutorSetsSkinnedMeshComponentIfMeshTypeIsSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createSkinnedMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::SkinnedMesh>(entity).handle,
            mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetMeshTest,
       ExecutorReplacesSkinnedMeshWithMeshIfNewAssetTypeIsMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createMesh();
  activeScene().entityDatabase.set<quoll::SkinnedMesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Mesh>(entity).handle, mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetMeshTest,
       ExecutorReplacesMeshWithSkinnedMeshIfNewAssetTypeIsSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createMesh();
  auto mesh = createSkinnedMesh();
  activeScene().entityDatabase.set<quoll::Mesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::SkinnedMesh>(entity).handle,
            mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetMeshTest, UndoSetsPreviousMeshHandleIfSameComponent) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createMesh();
  auto mesh = createMesh();

  activeScene().entityDatabase.set<quoll::Mesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Mesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest, UndoSetsPreviousSkinnedMeshHandleIfSameComponent) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createSkinnedMesh();

  activeScene().entityDatabase.set<quoll::SkinnedMesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::SkinnedMesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest, UndoReplacesNewMeshWithOldSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createMesh();

  activeScene().entityDatabase.set<quoll::SkinnedMesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::SkinnedMesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest, UndoReplacesNewSkinnedMeshWithOldMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createMesh();
  auto mesh = createSkinnedMesh();

  activeScene().entityDatabase.set<quoll::Mesh>(entity, {oldMesh});

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Mesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest, UndoDeletesMeshComponentIfNoPreviousComponent) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest,
       UndoDeletesSkinnedMeshComponentIfNoPreviousComponent) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createSkinnedMesh();

  quoll::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest, PredicateReturnsTrueIfMeshExists) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createSkinnedMesh();

  EXPECT_TRUE(quoll::editor::EntitySetMesh(entity, mesh)
                  .predicate(state, assetRegistry));
}

TEST_P(EntitySetMeshTest, PredicateReturnsFalseIfMeshDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();

  EXPECT_FALSE(quoll::editor::EntitySetMesh(entity, quoll::MeshAssetHandle{25})
                   .predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetMeshTest);

using EntityDeleteMeshTest = EntityMeshActionTestBase;

TEST_P(EntityDeleteMeshTest, ExecutorDeletesMeshIfCurrentComponentIsMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createMesh();

  activeScene().entityDatabase.set<quoll::Mesh>(entity, {oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityDeleteMeshTest,
       ExecutorDeletesSkinnedMeshIfCurrentComponentIsSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createSkinnedMesh();

  activeScene().entityDatabase.set<quoll::SkinnedMesh>(entity, {oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_FALSE(activeScene().entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityDeleteMeshTest, UndoRecreatesMeshIfDeletedComponentWasMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createMesh();

  activeScene().entityDatabase.set<quoll::Mesh>(entity, {oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Mesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Mesh>(entity).handle,
            oldMesh);
}

TEST_P(EntityDeleteMeshTest,
       UndoRecreatesSkinnedMeshIfDeletedComponentWasSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createSkinnedMesh();

  activeScene().entityDatabase.set<quoll::SkinnedMesh>(entity, {oldMesh});
  quoll::editor::EntityDeleteMesh action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::SkinnedMesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::SkinnedMesh>(entity).handle,
            oldMesh);
}

TEST_P(EntityDeleteMeshTest, PredicateReturnsTrueIfEntityHasMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createMesh();
  activeScene().entityDatabase.set<quoll::Mesh>(entity, {mesh});

  EXPECT_TRUE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}

TEST_P(EntityDeleteMeshTest, PredicateReturnsTrueIfEntityHasSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createSkinnedMesh();
  activeScene().entityDatabase.set<quoll::SkinnedMesh>(entity, {mesh});

  EXPECT_TRUE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}

TEST_P(EntityDeleteMeshTest,
       PredicateReturnsFalseIfEntityHasNoMeshOrSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();

  EXPECT_FALSE(
      quoll::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteMeshTest);
