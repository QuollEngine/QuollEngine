#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityMeshRendererActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

// SetMeshRendererMaterialSlot
using EntitySetMeshRendererMaterialActionTest = ActionTestBase;

TEST_F(EntitySetMeshRendererMaterialActionTest,
       ExecutorSetsMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::MeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntitySetMeshRendererMaterial action(
      entity, 1, quoll::MaterialAssetHandle{5});
  auto res = action.onExecute(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::MeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 2);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer->materials.at(1), quoll::MaterialAssetHandle{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       UndoSetsPreviousMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::MeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntitySetMeshRendererMaterial action(
      entity, 1, quoll::MaterialAssetHandle{5});
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::MeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 2);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer->materials.at(1), quoll::MaterialAssetHandle{2});
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfNoMeshRendererComponent) {
  auto entity = state.scene.entityDatabase.entity();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(quoll::editor::EntitySetMeshRendererMaterial(entity, 0, material)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfSlotIsBiggerThanExistingSlots) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::MeshRenderer renderer{materials};
    entity.set(renderer);
  }

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_FALSE(quoll::editor::EntitySetMeshRendererMaterial(entity, 5, material)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::MeshRenderer renderer{materials};
    entity.set(renderer);
  }

  EXPECT_FALSE(quoll::editor::EntitySetMeshRendererMaterial(
                   entity, 5, quoll::MaterialAssetHandle{25})
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsTrueIfSlotIsValidAndMaterialExists) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::MeshRenderer renderer{materials};
    entity.set(renderer);
  }

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_TRUE(quoll::editor::EntitySetMeshRendererMaterial(entity, 1, material)
                  .predicate(state, assetRegistry));
}

// CreateMeshRendererMaterialSlot
using EntityAddMeshRendererMaterialSlotActionTest = ActionTestBase;

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       ExecutorCreatesNewSlotInMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::MeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntityAddMeshRendererMaterialSlot action(
      entity, quoll::MaterialAssetHandle{5});
  auto res = action.onExecute(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::MeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 3);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer->materials.at(1), quoll::MaterialAssetHandle{2});
  EXPECT_EQ(renderer->materials.at(2), quoll::MaterialAssetHandle{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       UndoRemovesLastSlotFromMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::MeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntityAddMeshRendererMaterialSlot action(
      entity, quoll::MaterialAssetHandle{5});
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::MeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 2);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer->materials.at(1), quoll::MaterialAssetHandle{2});
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoMeshRenderer) {
  auto entity = state.scene.entityDatabase.entity();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      quoll::editor::EntityAddMeshRendererMaterialSlot(entity, material)
          .predicate(state, assetRegistry));
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::MeshRenderer renderer{};
  entity.set(renderer);

  EXPECT_FALSE(quoll::editor::EntityAddMeshRendererMaterialSlot(
                   entity, quoll::MaterialAssetHandle{25})
                   .predicate(state, assetRegistry));
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMaterialExists) {
  auto entity = state.scene.entityDatabase.entity();

  quoll::MeshRenderer renderer{};
  entity.set(renderer);

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_TRUE(quoll::editor::EntityAddMeshRendererMaterialSlot(entity, material)
                  .predicate(state, assetRegistry));
}

// EntityRemoveLastMeshRendererMaterialSlot
using EntityRemoveLastMeshRendererMaterialSlotActionTest = ActionTestBase;

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       ExecutorRemovesLastSlotFromMeshRenderer) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::MeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntityRemoveLastMeshRendererMaterialSlot action(entity);
  auto res = action.onExecute(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::MeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 1);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       UndoAddsLastRemovedMaterialToEnd) {
  auto entity = state.scene.entityDatabase.entity();

  {
    std::vector<quoll::MaterialAssetHandle> materials{
        quoll::MaterialAssetHandle{1}, quoll::MaterialAssetHandle{2}};
    quoll::MeshRenderer renderer{materials};
    entity.set(renderer);
  }

  quoll::editor::EntityRemoveLastMeshRendererMaterialSlot action(entity);
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  auto renderer = entity.get_ref<quoll::MeshRenderer>();

  EXPECT_EQ(renderer->materials.size(), 2);
  EXPECT_EQ(renderer->materials.at(0), quoll::MaterialAssetHandle{1});
  EXPECT_EQ(renderer->materials.at(1), quoll::MaterialAssetHandle{2});
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoMeshRenderer) {
  auto entity = state.scene.entityDatabase.entity();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastMeshRendererMaterialSlot(entity).predicate(
          state, assetRegistry));
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMeshRendererHasNoMaterialSlots) {
  auto entity = state.scene.entityDatabase.entity();
  entity.set<quoll::MeshRenderer>({});

  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastMeshRendererMaterialSlot(entity).predicate(
          state, assetRegistry));
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMeshRendererHasMaterials) {
  auto entity = state.scene.entityDatabase.entity();

  std::vector<quoll::MaterialAssetHandle> materials{
      quoll::MaterialAssetHandle{1}};
  quoll::MeshRenderer renderer{materials};
  entity.set(renderer);

  EXPECT_TRUE(
      quoll::editor::EntityRemoveLastMeshRendererMaterialSlot(entity).predicate(
          state, assetRegistry));
}
