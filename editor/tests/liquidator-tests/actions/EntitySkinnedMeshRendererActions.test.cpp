#include "liquid/core/Base.h"
#include "liquidator/actions/EntitySkinnedMeshRendererActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

// SetSkinnedMeshRendererMaterialSlot
using EntitySetSkinnedMeshRendererMaterialActionTest = ActionTestBase;

TEST_P(EntitySetSkinnedMeshRendererMaterialActionTest,
       ExecutorSetsMaterialInSpecifiedSlot) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntitySetSkinnedMeshRendererMaterial action(
      entity, 1, quoll::MaterialAssetHandle{5});
  auto res = action.onExecute(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer.materials.at(1), quoll::MaterialAssetHandle{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntitySetSkinnedMeshRendererMaterialActionTest,
       UndoSetsPreviousMaterialInSpecifiedSlot) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntitySetSkinnedMeshRendererMaterial action(
      entity, 1, quoll::MaterialAssetHandle{5});
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer.materials.at(1), quoll::MaterialAssetHandle{2});
}

TEST_P(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRendererComponent) {
  auto entity = activeScene().entityDatabase.create();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 0, material)
          .predicate(state, assetRegistry));
}

TEST_P(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfSlotIsBiggerThanExistingSlots) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_FALSE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 5, material)
          .predicate(state, assetRegistry));
}

TEST_P(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  EXPECT_FALSE(quoll::editor::EntitySetSkinnedMeshRendererMaterial(
                   entity, 5, quoll::MaterialAssetHandle{25})
                   .predicate(state, assetRegistry));
}

TEST_P(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsTrueIfSlotIsValidAndMaterialExists) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_TRUE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 1, material)
          .predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest,
                     EntitySetSkinnedMeshRendererMaterialActionTest);

// CreateSkinnedMeshRendererMaterialSlot
using EntityAddSkinnedMeshRendererMaterialSlotActionTest = ActionTestBase;

TEST_P(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       ExecutorCreatesNewSlotInSkinnedMeshRendererMaterials) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot action(
      entity, quoll::MaterialAssetHandle{5});
  auto res = action.onExecute(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 3);
  EXPECT_EQ(renderer.materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer.materials.at(1), quoll::MaterialAssetHandle{2});
  EXPECT_EQ(renderer.materials.at(2), quoll::MaterialAssetHandle{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       UndoRemovesLastSlotFromSkinnedMeshRendererMaterials) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot action(
      entity, quoll::MaterialAssetHandle{5});
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer.materials.at(1), quoll::MaterialAssetHandle{2});
}

TEST_P(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRenderer) {
  auto entity = activeScene().entityDatabase.create();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(entity, material)
          .predicate(state, assetRegistry));
}

TEST_P(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = activeScene().entityDatabase.create();

  quoll::SkinnedMeshRenderer renderer{};
  activeScene().entityDatabase.set(entity, renderer);

  EXPECT_FALSE(quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(
                   entity, quoll::MaterialAssetHandle{25})
                   .predicate(state, assetRegistry));
}

TEST_P(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMaterialExists) {
  auto entity = activeScene().entityDatabase.create();

  quoll::SkinnedMeshRenderer renderer{};
  activeScene().entityDatabase.set(entity, renderer);

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_TRUE(
      quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(entity, material)
          .predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest,
                     EntityAddSkinnedMeshRendererMaterialSlotActionTest);

// EntityRemoveLastSkinnedMeshRendererMaterialSlot
using EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest =
    ActionTestBase;

TEST_P(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       ExecutorRemovesLastSlotFromSkinnedMeshRenderer) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot action(entity);
  auto res = action.onExecute(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 1);
  EXPECT_EQ(renderer.materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       UndoAddsLastRemovedMaterialToEnd) {
  auto entity = activeScene().entityDatabase.create();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    activeScene().entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot action(entity);
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  const auto &renderer =
      activeScene().entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer.materials.at(1), quoll::MaterialAssetHandle{2});
}

TEST_P(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRenderer) {
  auto entity = activeScene().entityDatabase.create();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot(entity)
          .predicate(state, assetRegistry));
}

TEST_P(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfSkinnedMeshRendererHasNoMaterialSlots) {
  auto entity = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::SkinnedMeshRenderer>(entity, {});

  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot(entity)
          .predicate(state, assetRegistry));
}

TEST_P(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfSkinnedMeshRendererHasMaterials) {
  auto entity = activeScene().entityDatabase.create();

  std::vector<quoll::MaterialAssetHandle> materials{
      quoll::MaterialAssetHandle{1}};
  quoll::SkinnedMeshRenderer renderer{materials};
  activeScene().entityDatabase.set(entity, renderer);

  EXPECT_TRUE(
      quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot(entity)
          .predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest,
                     EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest);

// Default tests
using EntityCreateSkinnedMeshRendererActionTest = ActionTestBase;
InitDefaultCreateComponentTests(EntityCreateSkinnedMeshRendererActionTest,
                                EntityCreateSkinnedMeshRenderer,
                                SkinnedMeshRenderer);
InitActionsTestSuite(EntityActionsTest,
                     EntityCreateSkinnedMeshRendererActionTest);

using EntityDeleteSkinnedMeshRendererActionTest = ActionTestBase;

InitDefaultDeleteComponentTests(EntityDeleteSkinnedMeshRendererActionTest,
                                EntityDeleteSkinnedMeshRenderer,
                                SkinnedMeshRenderer);
InitActionsTestSuite(EntityActionsTest,
                     EntityDeleteSkinnedMeshRendererActionTest);
