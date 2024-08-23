#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntitySkinnedMeshRendererActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

// SetSkinnedMeshRendererMaterialSlot
using EntitySetSkinnedMeshRendererMaterialActionTest = ActionTestBase;

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       ExecutorSetsMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntitySetSkinnedMeshRendererMaterial action(
      entity, 1, quoll::MaterialAssetHandle{5});
  auto res = action.onExecute(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::SkinnedMeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 2);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer->materials.at(1), quoll::MaterialAssetHandle{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       UndoSetsPreviousMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntitySetSkinnedMeshRendererMaterial action(
      entity, 1, quoll::MaterialAssetHandle{5});
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::SkinnedMeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 2);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer->materials.at(1), quoll::MaterialAssetHandle{2});
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRendererComponent) {
  auto entity = state.scene.entityDatabase.entity();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 0, material)
          .predicate(state, assetRegistry));
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfSlotIsBiggerThanExistingSlots) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    entity.set(renderer);
  }

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_FALSE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 5, material)
          .predicate(state, assetRegistry));
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    entity.set(renderer);
  }

  EXPECT_FALSE(quoll::editor::EntitySetSkinnedMeshRendererMaterial(
                   entity, 5, quoll::MaterialAssetHandle{25})
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsTrueIfSlotIsValidAndMaterialExists) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    entity.set(renderer);
  }

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_TRUE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 1, material)
          .predicate(state, assetRegistry));
}

// CreateSkinnedMeshRendererMaterialSlot
using EntityAddSkinnedMeshRendererMaterialSlotActionTest = ActionTestBase;

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       ExecutorCreatesNewSlotInSkinnedMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot action(
      entity, quoll::MaterialAssetHandle{5});
  auto res = action.onExecute(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::SkinnedMeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 3);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer->materials.at(1), quoll::MaterialAssetHandle{2});
  EXPECT_EQ(renderer->materials.at(2), quoll::MaterialAssetHandle{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       UndoRemovesLastSlotFromSkinnedMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot action(
      entity, quoll::MaterialAssetHandle{5});
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::SkinnedMeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 2);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer->materials.at(1), quoll::MaterialAssetHandle{2});
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRenderer) {
  auto entity = state.scene.entityDatabase.entity();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(entity, material)
          .predicate(state, assetRegistry));
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::SkinnedMeshRenderer renderer{};
  entity.set(renderer);

  EXPECT_FALSE(quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(
                   entity, quoll::MaterialAssetHandle{25})
                   .predicate(state, assetRegistry));
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMaterialExists) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::SkinnedMeshRenderer renderer{};
  entity.set(renderer);

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_TRUE(
      quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(entity, material)
          .predicate(state, assetRegistry));
}

// EntityRemoveLastSkinnedMeshRendererMaterialSlot
using EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest =
    ActionTestBase;

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       ExecutorRemovesLastSlotFromSkinnedMeshRenderer) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot action(entity);
  auto res = action.onExecute(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::SkinnedMeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 1);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       UndoAddsLastRemovedMaterialToEnd) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot action(entity);
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::SkinnedMeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 2);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer->materials.at(1), quoll::MaterialAssetHandle{2});
}

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRenderer) {
  auto entity = state.scene.entityDatabase.entity();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot(entity)
          .predicate(state, assetRegistry));
}

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfSkinnedMeshRendererHasNoMaterialSlots) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::SkinnedMeshRenderer>({});

  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot(entity)
          .predicate(state, assetRegistry));
}

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfSkinnedMeshRendererHasMaterials) {
  auto entity = state.scene.entityDatabase.entity();

  std::vector<quoll::MaterialAssetHandle> materials{
      quoll::MaterialAssetHandle{1}};
  quoll::SkinnedMeshRenderer renderer{materials};
  entity.set(renderer);

  EXPECT_TRUE(
      quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot(entity)
          .predicate(state, assetRegistry));
}
