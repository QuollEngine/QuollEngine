#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityMeshRendererActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

// SetMeshRendererMaterialSlot
using EntitySetMeshRendererMaterialActionTest = ActionTestBase;

TEST_F(EntitySetMeshRendererMaterialActionTest,
       ExecutorSetsMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntitySetMeshRendererMaterial action(
      entity, 1, quoll::AssetHandle<quoll::MaterialAsset>{5});
  auto res = action.onExecute(state, assetRegistry);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(renderer.materials.at(1),
            quoll::AssetHandle<quoll::MaterialAsset>{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       UndoSetsPreviousMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntitySetMeshRendererMaterial action(
      entity, 1, quoll::AssetHandle<quoll::MaterialAsset>{5});
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(renderer.materials.at(1),
            quoll::AssetHandle<quoll::MaterialAsset>{2});
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfNoMeshRendererComponent) {
  auto entity = state.scene.entityDatabase.create();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(quoll::editor::EntitySetMeshRendererMaterial(entity, 0, material)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfSlotIsBiggerThanExistingSlots) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_FALSE(quoll::editor::EntitySetMeshRendererMaterial(entity, 5, material)
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  EXPECT_FALSE(quoll::editor::EntitySetMeshRendererMaterial(
                   entity, 5, quoll::AssetHandle<quoll::MaterialAsset>{25})
                   .predicate(state, assetRegistry));
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsTrueIfSlotIsValidAndMaterialExists) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_TRUE(quoll::editor::EntitySetMeshRendererMaterial(entity, 1, material)
                  .predicate(state, assetRegistry));
}

// CreateMeshRendererMaterialSlot
using EntityAddMeshRendererMaterialSlotActionTest = ActionTestBase;

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       ExecutorCreatesNewSlotInMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityAddMeshRendererMaterialSlot action(
      entity, quoll::AssetHandle<quoll::MaterialAsset>{5});
  auto res = action.onExecute(state, assetRegistry);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 3);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(renderer.materials.at(1),
            quoll::AssetHandle<quoll::MaterialAsset>{2});
  EXPECT_EQ(renderer.materials.at(2),
            quoll::AssetHandle<quoll::MaterialAsset>{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       UndoRemovesLastSlotFromMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityAddMeshRendererMaterialSlot action(
      entity, quoll::AssetHandle<quoll::MaterialAsset>{5});
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(renderer.materials.at(1),
            quoll::AssetHandle<quoll::MaterialAsset>{2});
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      quoll::editor::EntityAddMeshRendererMaterialSlot(entity, material)
          .predicate(state, assetRegistry));
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  quoll::MeshRenderer renderer{};
  state.scene.entityDatabase.set(entity, renderer);

  EXPECT_FALSE(quoll::editor::EntityAddMeshRendererMaterialSlot(
                   entity, quoll::AssetHandle<quoll::MaterialAsset>{25})
                   .predicate(state, assetRegistry));
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMaterialExists) {
  auto entity = state.scene.entityDatabase.create();

  quoll::MeshRenderer renderer{};
  state.scene.entityDatabase.set(entity, renderer);

  auto material = assetRegistry.getMaterials().addAsset({});

  EXPECT_TRUE(quoll::editor::EntityAddMeshRendererMaterialSlot(entity, material)
                  .predicate(state, assetRegistry));
}

// EntityRemoveLastMeshRendererMaterialSlot
using EntityRemoveLastMeshRendererMaterialSlotActionTest = ActionTestBase;

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       ExecutorRemovesLastSlotFromMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityRemoveLastMeshRendererMaterialSlot action(entity);
  auto res = action.onExecute(state, assetRegistry);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 1);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       UndoAddsLastRemovedMaterialToEnd) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityRemoveLastMeshRendererMaterialSlot action(entity);
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(renderer.materials.at(1),
            quoll::AssetHandle<quoll::MaterialAsset>{2});
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();
  auto material = assetRegistry.getMaterials().addAsset({});
  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastMeshRendererMaterialSlot(entity).predicate(
          state, assetRegistry));
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMeshRendererHasNoMaterialSlots) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::MeshRenderer>(entity, {});

  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastMeshRendererMaterialSlot(entity).predicate(
          state, assetRegistry));
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMeshRendererHasMaterials) {
  auto entity = state.scene.entityDatabase.create();

  std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
      quoll::AssetHandle<quoll::MaterialAsset>{1}};
  quoll::MeshRenderer renderer{materials};
  state.scene.entityDatabase.set(entity, renderer);

  EXPECT_TRUE(
      quoll::editor::EntityRemoveLastMeshRendererMaterialSlot(entity).predicate(
          state, assetRegistry));
}
