#include "liquid/core/Base.h"
#include "liquidator/actions/EntityMeshActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using EntitySetMeshActionTest = ActionTestBase;

TEST_P(EntitySetMeshActionTest, ExecutorSetsMeshForEntity) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = state.assetRegistry.getMeshes().addAsset({});

  liquid::editor::EntitySetMesh action(entity, mesh);
  auto res = action.onExecute(state);

  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Mesh>(entity).handle,
            mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetMeshActionTest, PredicateReturnsTrueIfAssetExists) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = state.assetRegistry.getMeshes().addAsset({});

  liquid::editor::EntitySetMesh action(entity, mesh);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntitySetMeshActionTest, PredicateReturnsFalseIfAssetDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetMesh action(entity, liquid::MeshAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntitySetMeshActionTest);

using EntityDeleteMeshActionTest = ActionTestBase;

TEST_P(EntityDeleteMeshActionTest, ExecutorDeletesMeshComponentFromEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Mesh>(entity, {});

  liquid::editor::EntityDeleteMesh action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Mesh>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteMeshActionTest,
       PredicateReturnsTrueIfEntityHasMeshComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Mesh>(entity, {});

  liquid::editor::EntityDeleteMesh action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntityDeleteMeshActionTest,
       PredicateReturnsTrueIfEntityHasNoMeshComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityDeleteMesh action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteMeshActionTest);

using EntitySetSkinnedMeshActionTest = ActionTestBase;

TEST_P(EntitySetSkinnedMeshActionTest, ExecutorSetsSkinnedMeshForEntity) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = state.assetRegistry.getSkinnedMeshes().addAsset({});

  liquid::editor::EntitySetSkinnedMesh action(entity, mesh);
  auto res = action.onExecute(state);

  EXPECT_EQ(
      activeScene().entityDatabase.get<liquid::SkinnedMesh>(entity).handle,
      mesh);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntitySetSkinnedMeshActionTest, PredicateReturnsTrueIfAssetExists) {
  auto entity = activeScene().entityDatabase.create();
  auto mesh = state.assetRegistry.getSkinnedMeshes().addAsset({});

  liquid::editor::EntitySetSkinnedMesh action(entity, mesh);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntitySetSkinnedMeshActionTest,
       PredicateReturnsFalseIfAssetDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntitySetSkinnedMesh action(
      entity, liquid::SkinnedMeshAssetHandle{15});
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntitySetSkinnedMeshActionTest);

using EntityDeleteSkinnedMeshActionTest = ActionTestBase;

TEST_P(EntityDeleteSkinnedMeshActionTest,
       ExecutorDeletesSkinnedMeshComponentFromEntity) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::SkinnedMesh>(entity, {});

  liquid::editor::EntityDeleteSkinnedMesh action(entity);
  auto res = action.onExecute(state);

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::SkinnedMesh>(entity));
  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
}

TEST_P(EntityDeleteSkinnedMeshActionTest,
       PredicateReturnsTrueIfEntityHasSkinnedMeshComponent) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::SkinnedMesh>(entity, {});

  liquid::editor::EntityDeleteSkinnedMesh action(entity);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(EntityDeleteSkinnedMeshActionTest,
       PredicateReturnsTrueIfEntityHasNoSkinnedMeshComponent) {
  auto entity = activeScene().entityDatabase.create();

  liquid::editor::EntityDeleteSkinnedMesh action(entity);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, EntityDeleteSkinnedMeshActionTest);
