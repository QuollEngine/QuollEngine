#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntitySkinnedMeshRendererActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

// SetSkinnedMeshRendererMaterialSlot
using EntitySetSkinnedMeshRendererMaterialActionTest = ActionTestBase;

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       ExecutorSetsMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntitySetSkinnedMeshRendererMaterial action(
      entity, 1, quoll::AssetHandle<quoll::MaterialAsset>{5});
  auto res = action.onExecute(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(renderer.materials.at(1),
            quoll::AssetHandle<quoll::MaterialAsset>{5});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       UndoSetsPreviousMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntitySetSkinnedMeshRendererMaterial action(
      entity, 1, quoll::AssetHandle<quoll::MaterialAsset>{5});
  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(renderer.materials.at(1),
            quoll::AssetHandle<quoll::MaterialAsset>{2});
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRendererComponent) {
  auto entity = state.scene.entityDatabase.create();
  auto material = assetCache.getRegistry().add<quoll::MaterialAsset>({});
  EXPECT_FALSE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 0, material)
          .predicate(state, assetCache));
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfSlotIsBiggerThanExistingSlots) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  auto material = assetCache.getRegistry().add<quoll::MaterialAsset>({});
  EXPECT_FALSE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 5, material)
          .predicate(state, assetCache));
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  EXPECT_FALSE(quoll::editor::EntitySetSkinnedMeshRendererMaterial(
                   entity, 5, quoll::AssetHandle<quoll::MaterialAsset>{25})
                   .predicate(state, assetCache));
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsTrueIfSlotIsValidAndMaterialExists) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  auto material = assetCache.getRegistry().add<quoll::MaterialAsset>({});
  EXPECT_TRUE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 1, material)
          .predicate(state, assetCache));
}

// CreateSkinnedMeshRendererMaterialSlot
using EntityAddSkinnedMeshRendererMaterialSlotActionTest = ActionTestBase;

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       ExecutorCreatesNewSlotInSkinnedMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot action(
      entity, quoll::AssetHandle<quoll::MaterialAsset>{5});
  auto res = action.onExecute(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

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

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       UndoRemovesLastSlotFromSkinnedMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot action(
      entity, quoll::AssetHandle<quoll::MaterialAsset>{5});
  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(renderer.materials.at(1),
            quoll::AssetHandle<quoll::MaterialAsset>{2});
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();
  auto material = assetCache.getRegistry().add<quoll::MaterialAsset>({});
  EXPECT_FALSE(
      quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(entity, material)
          .predicate(state, assetCache));
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  quoll::SkinnedMeshRenderer renderer{};
  state.scene.entityDatabase.set(entity, renderer);

  EXPECT_FALSE(quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(
                   entity, quoll::AssetHandle<quoll::MaterialAsset>{25})
                   .predicate(state, assetCache));
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMaterialExists) {
  auto entity = state.scene.entityDatabase.create();

  quoll::SkinnedMeshRenderer renderer{};
  state.scene.entityDatabase.set(entity, renderer);

  auto material = assetCache.getRegistry().add<quoll::MaterialAsset>({});
  EXPECT_TRUE(
      quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(entity, material)
          .predicate(state, assetCache));
}

// EntityRemoveLastSkinnedMeshRendererMaterialSlot
using EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest =
    ActionTestBase;

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       ExecutorRemovesLastSlotFromSkinnedMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot action(entity);
  auto res = action.onExecute(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 1);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       UndoAddsLastRemovedMaterialToEnd) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
        quoll::AssetHandle<quoll::MaterialAsset>{1},
        quoll::AssetHandle<quoll::MaterialAsset>{2}};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot action(entity);
  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0),
            quoll::AssetHandle<quoll::MaterialAsset>{1});
  EXPECT_EQ(renderer.materials.at(1),
            quoll::AssetHandle<quoll::MaterialAsset>{2});
}

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();
  auto material = assetCache.getRegistry().add<quoll::MaterialAsset>({});
  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot(entity)
          .predicate(state, assetCache));
}

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfSkinnedMeshRendererHasNoMaterialSlots) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::SkinnedMeshRenderer>(entity, {});

  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot(entity)
          .predicate(state, assetCache));
}

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfSkinnedMeshRendererHasMaterials) {
  auto entity = state.scene.entityDatabase.create();

  std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials{
      quoll::AssetHandle<quoll::MaterialAsset>{1}};
  quoll::SkinnedMeshRenderer renderer{materials};
  state.scene.entityDatabase.set(entity, renderer);

  EXPECT_TRUE(
      quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot(entity)
          .predicate(state, assetCache));
}
