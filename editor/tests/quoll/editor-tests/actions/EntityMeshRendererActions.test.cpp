#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntityMeshRendererActions.h"
#include "quoll-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

class EntityMeshRendererActionTestBase : public ActionTestBase {
public:
  EntityMeshRendererActionTestBase() {
    mat1 = createAsset<quoll::MaterialAsset>();
    mat2 = createAsset<quoll::MaterialAsset>();
    mat3 = createAsset<quoll::MaterialAsset>();
  }

  quoll::AssetRef<quoll::MaterialAsset> mat1;
  quoll::AssetRef<quoll::MaterialAsset> mat2;
  quoll::AssetRef<quoll::MaterialAsset> mat3;
};

// SetMeshRendererMaterialSlot
using EntitySetMeshRendererMaterialActionTest =
    EntityMeshRendererActionTestBase;

TEST_F(EntitySetMeshRendererMaterialActionTest,
       ExecutorSetsMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntitySetMeshRendererMaterial action(entity, 1, mat3);
  auto res = action.onExecute(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(renderer.materials.at(1), mat3.handle());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       UndoSetsPreviousMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntitySetMeshRendererMaterial action(entity, 1, mat3);
  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(renderer.materials.at(1), mat2.handle());
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfNoMeshRendererComponent) {
  auto entity = state.scene.entityDatabase.create();
  EXPECT_FALSE(quoll::editor::EntitySetMeshRendererMaterial(entity, 0, mat1)
                   .predicate(state, assetCache));
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfSlotIsBiggerThanExistingSlots) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  EXPECT_FALSE(quoll::editor::EntitySetMeshRendererMaterial(entity, 5, mat3)
                   .predicate(state, assetCache));
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  EXPECT_FALSE(quoll::editor::EntitySetMeshRendererMaterial(
                   entity, 1, quoll::AssetRef<quoll::MaterialAsset>())
                   .predicate(state, assetCache));
}

TEST_F(EntitySetMeshRendererMaterialActionTest,
       PredicateReturnsTrueIfSlotIsValidAndMaterialExists) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  EXPECT_TRUE(quoll::editor::EntitySetMeshRendererMaterial(entity, 1, mat3)
                  .predicate(state, assetCache));
}

// CreateMeshRendererMaterialSlot
using EntityAddMeshRendererMaterialSlotActionTest =
    EntityMeshRendererActionTestBase;

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       ExecutorCreatesNewSlotInMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityAddMeshRendererMaterialSlot action(entity, mat3);
  auto res = action.onExecute(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 3);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(renderer.materials.at(1), mat2.handle());
  EXPECT_EQ(renderer.materials.at(2), mat3.handle());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       UndoRemovesLastSlotFromMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityAddMeshRendererMaterialSlot action(entity, mat3);
  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(renderer.materials.at(1), mat2.handle());
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();
  EXPECT_FALSE(quoll::editor::EntityAddMeshRendererMaterialSlot(entity, mat1)
                   .predicate(state, assetCache));
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  quoll::MeshRenderer renderer{};
  state.scene.entityDatabase.set(entity, renderer);

  EXPECT_FALSE(quoll::editor::EntityAddMeshRendererMaterialSlot(
                   entity, quoll::AssetRef<quoll::MaterialAsset>())
                   .predicate(state, assetCache));
}

TEST_F(EntityAddMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMaterialExists) {
  auto entity = state.scene.entityDatabase.create();

  quoll::MeshRenderer renderer{};
  state.scene.entityDatabase.set(entity, renderer);

  EXPECT_TRUE(quoll::editor::EntityAddMeshRendererMaterialSlot(entity, mat1)
                  .predicate(state, assetCache));
}

// EntityRemoveLastMeshRendererMaterialSlot
using EntityRemoveLastMeshRendererMaterialSlotActionTest =
    EntityMeshRendererActionTestBase;

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       ExecutorRemovesLastSlotFromMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityRemoveLastMeshRendererMaterialSlot action(entity);
  auto res = action.onExecute(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 1);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       UndoAddsLastRemovedMaterialToEnd) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::MeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityRemoveLastMeshRendererMaterialSlot action(entity);
  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::MeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(renderer.materials.at(1), mat2.handle());
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();
  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastMeshRendererMaterialSlot(entity).predicate(
          state, assetCache));
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMeshRendererHasNoMaterialSlots) {
  auto entity = state.scene.entityDatabase.create();
  state.scene.entityDatabase.set<quoll::MeshRenderer>(entity, {});

  EXPECT_FALSE(
      quoll::editor::EntityRemoveLastMeshRendererMaterialSlot(entity).predicate(
          state, assetCache));
}

TEST_F(EntityRemoveLastMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMeshRendererHasMaterials) {
  auto entity = state.scene.entityDatabase.create();

  std::vector materials{mat1};
  quoll::MeshRenderer renderer{materials};
  state.scene.entityDatabase.set(entity, renderer);

  EXPECT_TRUE(
      quoll::editor::EntityRemoveLastMeshRendererMaterialSlot(entity).predicate(
          state, assetCache));
}
