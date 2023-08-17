#include "liquid/core/Base.h"
#include "liquidator/actions/EntityMeshActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

class EntityMeshActionTestBase : public ActionTestBase {
public:
  liquid::MeshAssetHandle createMesh() {
    liquid::AssetData<liquid::MeshAsset> asset{};
    asset.type = liquid::AssetType::Mesh;
    return assetRegistry.getMeshes().addAsset(asset);
  }

  liquid::MeshAssetHandle createSkinnedMesh() {
    liquid::AssetData<liquid::MeshAsset> asset{};
    asset.type = liquid::AssetType::SkinnedMesh;
    return assetRegistry.getMeshes().addAsset(asset);
  }
};

using EntitySetMeshTest = EntityMeshActionTestBase;

TEST_P(EntitySetMeshTest, ExecutorSetsMeshComponentIfMeshTypeIsMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createMesh();

  liquid::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Mesh>(entity).handle,
            mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetMeshTest,
       ExecutorSetsSkinnedMeshComponentIfMeshTypeIsSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createSkinnedMesh();

  liquid::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::SkinnedMesh>(entity).handle,
      mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetMeshTest,
       ExecutorReplacesSkinnedMeshWithMeshIfNewAssetTypeIsMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createMesh();
  activeScene().entityDatabase.set<liquid::SkinnedMesh>(entity, {oldMesh});

  liquid::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Mesh>(entity).handle,
            mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetMeshTest,
       ExecutorReplacesMeshWithSkinnedMeshIfNewAssetTypeIsSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createMesh();
  auto mesh = createSkinnedMesh();
  activeScene().entityDatabase.set<liquid::Mesh>(entity, {oldMesh});

  liquid::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::SkinnedMesh>(entity).handle,
      mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetMeshTest, UndoSetsPreviousMeshHandleIfSameComponent) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createMesh();
  auto mesh = createMesh();

  activeScene().entityDatabase.set<liquid::Mesh>(entity, {oldMesh});

  liquid::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Mesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest, UndoSetsPreviousSkinnedMeshHandleIfSameComponent) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createSkinnedMesh();

  activeScene().entityDatabase.set<liquid::SkinnedMesh>(entity, {oldMesh});

  liquid::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::SkinnedMesh>(entity).handle,
      oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest, UndoReplacesNewMeshWithOldSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createSkinnedMesh();
  auto mesh = createMesh();

  activeScene().entityDatabase.set<liquid::SkinnedMesh>(entity, {oldMesh});

  liquid::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::SkinnedMesh>(entity).handle,
      oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest, UndoReplacesNewSkinnedMeshWithOldMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createMesh();
  auto mesh = createSkinnedMesh();

  activeScene().entityDatabase.set<liquid::Mesh>(entity, {oldMesh});

  liquid::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Mesh>(entity).handle,
            oldMesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest, UndoDeletesMeshComponentIfNoPreviousComponent) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createMesh();

  liquid::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest,
       UndoDeletesSkinnedMeshComponentIfNoPreviousComponent) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createSkinnedMesh();

  liquid::editor::EntitySetMesh action(entity, mesh);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshTest, PredicateReturnsTrueIfMeshExists) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createSkinnedMesh();

  EXPECT_TRUE(liquid::editor::EntitySetMesh(entity, mesh)
                  .predicate(state, assetRegistry));
}

TEST_P(EntitySetMeshTest, PredicateReturnsFalseIfMeshDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();

  EXPECT_FALSE(
      liquid::editor::EntitySetMesh(entity, liquid::MeshAssetHandle{25})
          .predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntitySetMeshTest);

using EntityDeleteMeshTest = EntityMeshActionTestBase;

TEST_P(EntityDeleteMeshTest, ExecutorDeletesMeshIfCurrentComponentIsMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createMesh();

  activeScene().entityDatabase.set<liquid::Mesh>(entity, {oldMesh});
  liquid::editor::EntityDeleteMesh action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityDeleteMeshTest,
       ExecutorDeletesSkinnedMeshIfCurrentComponentIsSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createSkinnedMesh();

  activeScene().entityDatabase.set<liquid::SkinnedMesh>(entity, {oldMesh});
  liquid::editor::EntityDeleteMesh action(entity);
  auto res = action.onExecute(state, assetRegistry);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityDeleteMeshTest, UndoRecreatesMeshIfDeletedComponentWasMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createMesh();

  activeScene().entityDatabase.set<liquid::Mesh>(entity, {oldMesh});
  liquid::editor::EntityDeleteMesh action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Mesh>(entity).handle,
            oldMesh);
}

TEST_P(EntityDeleteMeshTest,
       UndoRecreatesSkinnedMeshIfDeletedComponentWasSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto oldMesh = createSkinnedMesh();

  activeScene().entityDatabase.set<liquid::SkinnedMesh>(entity, {oldMesh});
  liquid::editor::EntityDeleteMesh action(entity);
  action.onExecute(state, assetRegistry);
  auto res = action.onUndo(state, assetRegistry);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::SkinnedMesh>(entity).handle,
      oldMesh);
}

TEST_P(EntityDeleteMeshTest, PredicateReturnsTrueIfEntityHasMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createMesh();
  activeScene().entityDatabase.set<liquid::Mesh>(entity, {mesh});

  EXPECT_TRUE(
      liquid::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}

TEST_P(EntityDeleteMeshTest, PredicateReturnsTrueIfEntityHasSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = createSkinnedMesh();
  activeScene().entityDatabase.set<liquid::SkinnedMesh>(entity, {mesh});

  EXPECT_TRUE(
      liquid::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}

TEST_P(EntityDeleteMeshTest,
       PredicateReturnsFalseIfEntityHasNoMeshOrSkinnedMesh) {
  auto entity = activeScene().entityDatabase.create();

  EXPECT_FALSE(
      liquid::editor::EntityDeleteMesh(entity).predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteMeshTest);
