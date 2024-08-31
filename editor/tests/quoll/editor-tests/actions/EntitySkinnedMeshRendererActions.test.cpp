#include "quoll/core/Base.h"
#include "quoll/editor/actions/EntitySkinnedMeshRendererActions.h"
#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"
#include "DefaultEntityTests.h"

class EntitySkinnedMeshRendererActionTestBase : public ActionTestBase {
public:
  EntitySkinnedMeshRendererActionTestBase() {
    quoll::AssetData<quoll::MaterialAsset> m1;
    m1.uuid = quoll::Uuid::generate();
    assetCache.getRegistry().add(m1);
    mat1 = assetCache.request<quoll::MaterialAsset>(m1.uuid).data();

    quoll::AssetData<quoll::MaterialAsset> m2;
    m2.uuid = quoll::Uuid::generate();
    assetCache.getRegistry().add(m2);
    mat2 = assetCache.request<quoll::MaterialAsset>(m1.uuid).data();

    quoll::AssetData<quoll::MaterialAsset> m3;
    m3.uuid = quoll::Uuid::generate();
    assetCache.getRegistry().add(m3);
    mat3 = assetCache.request<quoll::MaterialAsset>(m1.uuid).data();
  }

  quoll::AssetRef<quoll::MaterialAsset> mat1;
  quoll::AssetRef<quoll::MaterialAsset> mat2;
  quoll::AssetRef<quoll::MaterialAsset> mat3;
};

// SetSkinnedMeshRendererMaterialSlot
using EntitySetSkinnedMeshRendererMaterialActionTest =
    EntitySkinnedMeshRendererActionTestBase;

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       ExecutorSetsMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntitySetSkinnedMeshRendererMaterial action(entity, 1, mat3);
  auto res = action.onExecute(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(renderer.materials.at(1), mat3.handle());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       UndoSetsPreviousMaterialInSpecifiedSlot) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntitySetSkinnedMeshRendererMaterial action(entity, 1, mat3);
  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(renderer.materials.at(1), mat2.handle());
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRendererComponent) {
  auto entity = state.scene.entityDatabase.create();
  EXPECT_FALSE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 0, mat1)
          .predicate(state, assetCache));
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfSlotIsBiggerThanExistingSlots) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  auto material = assetCache.getRegistry().add<quoll::MaterialAsset>({});
  EXPECT_FALSE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 5, mat3)
          .predicate(state, assetCache));
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  EXPECT_FALSE(quoll::editor::EntitySetSkinnedMeshRendererMaterial(
                   entity, 5, quoll::AssetRef<quoll::MaterialAsset>())
                   .predicate(state, assetCache));
}

TEST_F(EntitySetSkinnedMeshRendererMaterialActionTest,
       PredicateReturnsTrueIfSlotIsValidAndMaterialExists) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  EXPECT_TRUE(
      quoll::editor::EntitySetSkinnedMeshRendererMaterial(entity, 1, mat1)
          .predicate(state, assetCache));
}

// CreateSkinnedMeshRendererMaterialSlot
using EntityAddSkinnedMeshRendererMaterialSlotActionTest =
    EntitySkinnedMeshRendererActionTestBase;

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       ExecutorCreatesNewSlotInSkinnedMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot action(entity, mat3);
  auto res = action.onExecute(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 3);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(renderer.materials.at(1), mat2.handle());
  EXPECT_EQ(renderer.materials.at(2), mat3.handle());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       UndoRemovesLastSlotFromSkinnedMeshRendererMaterials) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot action(entity, mat3);
  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(renderer.materials.at(1), mat2.handle());
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfNoSkinnedMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();
  EXPECT_FALSE(
      quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(entity, mat1)
          .predicate(state, assetCache));
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsFalseIfMaterialDoesNotExist) {
  auto entity = state.scene.entityDatabase.create();

  quoll::SkinnedMeshRenderer renderer{};
  state.scene.entityDatabase.set(entity, renderer);

  EXPECT_FALSE(quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(
                   entity, quoll::AssetRef<quoll::MaterialAsset>())
                   .predicate(state, assetCache));
}

TEST_F(EntityAddSkinnedMeshRendererMaterialSlotActionTest,
       PredicateReturnsTrueIfMaterialExists) {
  auto entity = state.scene.entityDatabase.create();

  quoll::SkinnedMeshRenderer renderer{};
  state.scene.entityDatabase.set(entity, renderer);

  EXPECT_TRUE(
      quoll::editor::EntityAddSkinnedMeshRendererMaterialSlot(entity, mat1)
          .predicate(state, assetCache));
}

// EntityRemoveLastSkinnedMeshRendererMaterialSlot
using EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest =
    EntitySkinnedMeshRendererActionTestBase;

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       ExecutorRemovesLastSlotFromSkinnedMeshRenderer) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot action(entity);
  auto res = action.onExecute(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 1);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(res.entitiesToSave.at(0), entity);
  EXPECT_TRUE(res.addToHistory);
}

TEST_F(EntityRemoveLastSkinnedMeshRendererMaterialSlotActionTest,
       UndoAddsLastRemovedMaterialToEnd) {
  auto entity = state.scene.entityDatabase.create();

  {
    std::vector materials{mat1, mat2};
    quoll::SkinnedMeshRenderer renderer{materials};
    state.scene.entityDatabase.set(entity, renderer);
  }

  quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot action(entity);
  action.onExecute(state, assetCache);
  action.onUndo(state, assetCache);

  const auto &renderer =
      state.scene.entityDatabase.get<quoll::SkinnedMeshRenderer>(entity);

  EXPECT_EQ(renderer.materials.size(), 2);
  EXPECT_EQ(renderer.materials.at(0), mat1.handle());
  EXPECT_EQ(renderer.materials.at(1), mat2.handle());
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

  std::vector materials{mat1};
  quoll::SkinnedMeshRenderer renderer{materials};
  state.scene.entityDatabase.set(entity, renderer);

  EXPECT_TRUE(
      quoll::editor::EntityRemoveLastSkinnedMeshRendererMaterialSlot(entity)
          .predicate(state, assetCache));
}
