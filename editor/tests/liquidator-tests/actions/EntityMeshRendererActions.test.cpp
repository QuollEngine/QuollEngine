#include "liquid/core/Base.h"
#include "liquidator/actions/EntityMeshRendererActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

// SetMeshRendererMaterialSlot
using EntitySetMeshRendererMaterialActionTest = ActionTestBase;

TEST_P(EntitySetMeshRendererMaterialActionTest,
       ExecutorSetsMaterialInSpecifiedSlot) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<liquid::MaterialAssetHandle> materials{
        liquid::MaterialAssetHandle{1}, liquid::MaterialAssetHandle{2}};
    liquid::MeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  liquid::editor::EntitySetMeshRendererMaterial action(
      entity, 1, liquid::MaterialAssetHandle{5});
  auto res = action.onExecute(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<liquid::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), liquid::MaterialAssetHandle{1});
  EXPECT_EQ(renderer.materials.at(1), liquid::MaterialAssetHandle{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetMeshRendererMaterialActionTest,
       UndoSetsPreviousMaterialInSpecifiedSlot) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<liquid::MaterialAssetHandle> materials{
        liquid::MaterialAssetHandle{1}, liquid::MaterialAssetHandle{2}};
    liquid::MeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  liquid::editor::EntitySetMeshRendererMaterial action(
      entity, 1, liquid::MaterialAssetHandle{5});
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<liquid::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), liquid::MaterialAssetHandle{1});
  EXPECT_EQ(renderer.materials.at(1), liquid::MaterialAssetHandle{2});
}

TEST_P(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfNoMeshRendererComponent) {
  auto entity = activeScene().entityDatabase.create();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      liquid::editor::EntitySetMeshRendererMaterial(entity, 0, material)
          .predicate(state, assetRegistry));
}

TEST_P(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfSlotIsBiggerThanExistingSlots) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<liquid::MaterialAssetHandle> materials{
        liquid::MaterialAssetHandle{1}, liquid::MaterialAssetHandle{2}};
    liquid::MeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_FALSE(
      liquid::editor::EntitySetMeshRendererMaterial(entity, 5, material)
          .predicate(state, assetRegistry));
}

TEST_P(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<liquid::MaterialAssetHandle> materials{
        liquid::MaterialAssetHandle{1}, liquid::MaterialAssetHandle{2}};
    liquid::MeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  EXPECT_FALSE(liquid::editor::EntitySetMeshRendererMaterial(
                   entity, 5, liquid::MaterialAssetHandle{25})
                   .predicate(state, assetRegistry));
}

TEST_P(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsTrueIfSlotIsValidAndMaterialExists) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<liquid::MaterialAssetHandle> materials{
        liquid::MaterialAssetHandle{1}, liquid::MaterialAssetHandle{2}};
    liquid::MeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_TRUE(liquid::editor::EntitySetMeshRendererMaterial(entity, 1, material)
                  .predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest,
                     EntitySetMeshRendererMaterialActionTest);

// CreateMeshRendererMaterialSlot
using EntityAddMeshRendererMaterialSlotActionTest = ActionTestBase;

TEST_P(EntityAddMeshRendererMaterialSlotActionTest,
       ExecutorCreatesNewSlotInMeshRendererMaterials) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<liquid::MaterialAssetHandle> materials{
        liquid::MaterialAssetHandle{1}, liquid::MaterialAssetHandle{2}};
    liquid::MeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  liquid::editor::EntityAddMeshRendererMaterialSlot action(
      entity, liquid::MaterialAssetHandle{5});
  auto res = action.onExecute(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<liquid::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 3);
  EXPECT_EQ(renderer.materials.at(0), liquid::MaterialAssetHandle{1});
  EXPECT_EQ(renderer.materials.at(1), liquid::MaterialAssetHandle{2});
  EXPECT_EQ(renderer.materials.at(2), liquid::MaterialAssetHandle{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityAddMeshRendererMaterialSlotActionTest,
       UndoRemovesLastSlotFromMeshRendererMaterials) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<liquid::MaterialAssetHandle> materials{
        liquid::MaterialAssetHandle{1}, liquid::MaterialAssetHandle{2}};
    liquid::MeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  liquid::editor::EntityAddMeshRendererMaterialSlot action(
      entity, liquid::MaterialAssetHandle{5});
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<liquid::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), liquid::MaterialAssetHandle{1});
  EXPECT_EQ(renderer.materials.at(1), liquid::MaterialAssetHandle{2});
}

TEST_P(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoMeshRenderer) {
  auto entity = activeScene().entityDatabase.create();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      liquid::editor::EntityAddMeshRendererMaterialSlot(entity, material)
          .predicate(state, assetRegistry));
}

TEST_P(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();

  liquid::MeshRenderer renderer{};
  activeScene().entityDatabase.set(entity, renderer);

  EXPECT_FALSE(liquid::editor::EntityAddMeshRendererMaterialSlot(
                   entity, liquid::MaterialAssetHandle{25})
                   .predicate(state, assetRegistry));
}

TEST_P(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMaterialExists) {
  auto entity = activeScene().entityDatabase.create();

  liquid::MeshRenderer renderer{};
  activeScene().entityDatabase.set(entity, renderer);

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_TRUE(
      liquid::editor::EntityAddMeshRendererMaterialSlot(entity, material)
          .predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest,
                     EntityAddMeshRendererMaterialSlotActionTest);

// EntityRemoveLastMeshRendererMaterialSlot
using EntityRemoveLastMeshRendererMaterialSlotActionTest = ActionTestBase;

TEST_P(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       ExecutorRemovesLastSlotFromMeshRenderer) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<liquid::MaterialAssetHandle> materials{
        liquid::MaterialAssetHandle{1}, liquid::MaterialAssetHandle{2}};
    liquid::MeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  liquid::editor::EntityRemoveLastMeshRendererMaterialSlot action(entity);
  auto res = action.onExecute(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<liquid::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 1);
  EXPECT_EQ(renderer.materials.at(0), liquid::MaterialAssetHandle{1});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       UndoAddsLastRemovedMaterialToEnd) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<liquid::MaterialAssetHandle> materials{
        liquid::MaterialAssetHandle{1}, liquid::MaterialAssetHandle{2}};
    liquid::MeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  liquid::editor::EntityRemoveLastMeshRendererMaterialSlot action(entity);
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<liquid::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), liquid::MaterialAssetHandle{1});
  EXPECT_EQ(renderer.materials.at(1), liquid::MaterialAssetHandle{2});
}

TEST_P(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoMeshRenderer) {
  auto entity = activeScene().entityDatabase.create();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(liquid::editor::EntityRemoveLastMeshRendererMaterialSlot(entity)
                   .predicate(state, assetRegistry));
}

TEST_P(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMeshRendererHasNoMaterialSlots) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::MeshRenderer>(entity, {});

  EXPECT_FALSE(liquid::editor::EntityRemoveLastMeshRendererMaterialSlot(entity)
                   .predicate(state, assetRegistry));
}

TEST_P(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMeshRendererHasMaterials) {
  auto entity = activeScene().entityDatabase.create();

  std::vector<liquid::MaterialAssetHandle> materials{
      liquid::MaterialAssetHandle{1}};
  liquid::MeshRenderer renderer{materials};
  activeScene().entityDatabase.set(entity, renderer);

  EXPECT_TRUE(liquid::editor::EntityRemoveLastMeshRendererMaterialSlot(entity)
                  .predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest,
                     EntityRemoveLastMeshRendererMaterialSlotActionTest);

// Default tests
using EntityCreateMeshRendererActionTest = ActionTestBase;
InitDefaultCreateComponentTests(EntityCreateMeshRendererActionTest,
                                EntityCreateMeshRenderer, MeshRenderer);
InitActionsTestSuite(EntityActionsTest, EntityCreateMeshRendererActionTest);

using EntityDeleteMeshRendererActionTest = ActionTestBase;

InitDefaultDeleteComponentTests(EntityDeleteMeshRendererActionTest,
                                EntityDeleteMeshRenderer, MeshRenderer);
InitActionsTestSuite(EntityActionsTest, EntityDeleteMeshRendererActionTest);
