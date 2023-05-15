#include "liquid/core/Base.h"
#include "liquidator/actions/SpawnEntityActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using SpawnEmptyEntityAtViewActionTest = ActionTestBase;

TEST_P(SpawnEmptyEntityAtViewActionTest, ExecutorSpawnsEmptyEntityAtView) {
  auto camera = activeScene().entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  liquid::Camera component{};
  component.viewMatrix = viewMatrix;
  activeScene().entityDatabase.set(camera, component);
  state.camera = camera;

  liquid::editor::SpawnEmptyEntityAtView action;
  auto res = action.onExecute(state);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_NE(res.entitiesToSave.at(0), liquid::Entity::Null);

  auto entity = res.entitiesToSave.at(0);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Name>(entity).name,
            "New entity");
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::WorldTransform>(entity));
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SpawnEmptyEntityAtViewActionTest, UndoRemovesSpawnedEntity) {
  auto camera = activeScene().entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  liquid::Camera component{};
  component.viewMatrix = viewMatrix;
  activeScene().entityDatabase.set(camera, component);
  state.camera = camera;

  liquid::editor::SpawnEmptyEntityAtView action;
  auto execRes = action.onExecute(state);
  auto undoRes = action.onUndo(state);

  EXPECT_EQ(execRes.entitiesToSave.size(), 1);
  EXPECT_EQ(undoRes.entitiesToDelete.size(), 1);
  EXPECT_EQ(execRes.entitiesToSave.at(0), undoRes.entitiesToDelete.at(0));
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Delete>(
      execRes.entitiesToSave.at(0)));
}

TEST_P(SpawnEmptyEntityAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSpawnedEntityIsSelected) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  state.camera = camera;

  liquid::editor::SpawnEmptyEntityAtView action;
  auto res = action.onExecute(state);
  state.selectedEntity = res.entitiesToSave.at(0);

  action.onUndo(state);

  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(
    SpawnEmptyEntityAtViewActionTest,
    UndoSetsSelectedEntityToNullIfSelectedEntityIsADescendantOfSpawnedEntity) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  state.camera = camera;

  liquid::editor::SpawnEmptyEntityAtView action;
  auto res = action.onExecute(state);

  auto entity = res.entitiesToSave.at(0);

  {
    auto e1 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<liquid::Parent>(e1, {entity});

    auto e2 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<liquid::Parent>(e2, {e1});
    state.selectedEntity = e2;
  }

  action.onUndo(state);

  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(SpawnEmptyEntityAtViewActionTest,
       UndoDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotSpawnedEntity) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  state.camera = camera;
  state.selectedEntity = activeScene().entityDatabase.create();

  liquid::editor::SpawnEmptyEntityAtView action;
  action.onExecute(state);
  action.onUndo(state);

  EXPECT_NE(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(SpawnEmptyEntityAtViewActionTest,
       PredicateReturnsTrueIfCameraEntityHasCamera) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  state.camera = camera;

  liquid::editor::SpawnEmptyEntityAtView action;
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(SpawnEmptyEntityAtViewActionTest,
       PredicateReturnsFalseIfCameraEntityHasNoCamera) {
  auto camera = activeScene().entityDatabase.create();
  state.camera = camera;

  liquid::editor::SpawnEmptyEntityAtView action;
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, SpawnEmptyEntityAtViewActionTest);

using SpawnPrefabAtViewActionTest = ActionTestBase;

TEST_P(SpawnPrefabAtViewActionTest, ExecutorSpawnsPrefabAtView) {
  auto camera = activeScene().entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  liquid::Camera component{};
  component.viewMatrix = viewMatrix;
  activeScene().entityDatabase.set(camera, component);

  liquid::AssetData<liquid::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  asset.data.transforms.push_back({1});

  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state);

  ASSERT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_NE(res.entitiesToSave.at(0), liquid::Entity::Null);
  EXPECT_NE(res.entitiesToSave.at(1), liquid::Entity::Null);
  EXPECT_NE(res.entitiesToSave.at(2), liquid::Entity::Null);

  auto entity = res.entitiesToSave.at(2);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SpawnPrefabAtViewActionTest, UndoRemovesRootNode) {
  auto camera = activeScene().entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  liquid::Camera component{};
  component.viewMatrix = viewMatrix;
  activeScene().entityDatabase.set(camera, component);

  liquid::AssetData<liquid::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  asset.data.transforms.push_back({1});

  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtView action(prefab, camera);
  auto execRes = action.onExecute(state);

  auto undoRes = action.onUndo(state);

  ASSERT_EQ(execRes.entitiesToSave.size(), 3);
  ASSERT_EQ(undoRes.entitiesToDelete.size(), 1);

  auto entity = undoRes.entitiesToDelete.at(0);

  EXPECT_EQ(undoRes.entitiesToDelete.back(), entity);

  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Delete>(entity));
}

TEST_P(SpawnPrefabAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSpawnedRootIsSelected) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});

  liquid::AssetData<liquid::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state);

  auto entity = res.entitiesToSave.at(0);
  state.selectedEntity = entity;

  action.onUndo(state);
  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(SpawnPrefabAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSelectedEntityIsDescendantOfSpawnedRoot) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});

  liquid::AssetData<liquid::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  asset.data.transforms.push_back({1});
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state);

  state.selectedEntity = res.entitiesToSave.back();

  action.onUndo(state);
  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(
    SpawnEmptyEntityAtViewActionTest,
    UndoDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotInSpawnedPrefabTree) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});

  liquid::AssetData<liquid::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  asset.data.transforms.push_back({1});
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state);

  state.selectedEntity = liquid::Entity{25};

  action.onUndo(state);
  EXPECT_NE(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(
    SpawnPrefabAtViewActionTest,
    PredicateReturnsTrueIfPrefabAssetExistsAndIsNotEmptyAndCameraEntityHasCamera) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});

  liquid::AssetData<liquid::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtView action(prefab, camera);
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(SpawnPrefabAtViewActionTest,
       PredicateReturnsFalseIfPrefabAssetDoesNotExist) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  liquid::editor::SpawnPrefabAtView action(liquid::PrefabAssetHandle{15},
                                           camera);
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(SpawnPrefabAtViewActionTest, PredicateReturnsFalseIfPrefabAssetIsEmpty) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  liquid::AssetData<liquid::PrefabAsset> asset{};
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtView action(prefab, camera);
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(SpawnPrefabAtViewActionTest,
       PredicateReturnsFalseIfCameraEntityHasNoCamera) {
  auto camera = activeScene().entityDatabase.create();

  liquid::AssetData<liquid::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtView action(prefab, camera);
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, SpawnPrefabAtViewActionTest);

using SpawnSpriteAtViewActionTest = ActionTestBase;

TEST_P(SpawnSpriteAtViewActionTest, ExecutorSpawnsSpriteAtView) {
  liquid::AssetData<liquid::TextureAsset> data{};
  data.data.deviceHandle = liquid::rhi::TextureHandle{25};
  auto textureAsset = state.assetRegistry.getTextures().addAsset(data);

  auto camera = activeScene().entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  liquid::Camera component{};
  component.viewMatrix = viewMatrix;
  activeScene().entityDatabase.set(camera, component);
  state.camera = camera;

  liquid::editor::SpawnSpriteAtView action(textureAsset, camera);
  auto res = action.onExecute(state);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_NE(res.entitiesToSave.at(0), liquid::Entity::Null);

  auto entity = res.entitiesToSave.at(0);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));
  EXPECT_EQ(activeScene().entityDatabase.get<liquid::Name>(entity).name,
            "New sprite");
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::WorldTransform>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Sprite>(entity));

  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SpawnSpriteAtViewActionTest, UndoRemovesSpawnedEntity) {
  liquid::AssetData<liquid::TextureAsset> data{};
  data.data.deviceHandle = liquid::rhi::TextureHandle{25};
  auto textureAsset = state.assetRegistry.getTextures().addAsset(data);

  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  state.camera = camera;

  liquid::editor::SpawnSpriteAtView action(textureAsset, camera);
  auto execRes = action.onExecute(state);
  auto undoRes = action.onUndo(state);

  EXPECT_EQ(execRes.entitiesToSave.size(), 1);
  EXPECT_EQ(undoRes.entitiesToDelete.size(), 1);
  EXPECT_EQ(execRes.entitiesToSave.at(0), undoRes.entitiesToDelete.at(0));
  EXPECT_TRUE(activeScene().entityDatabase.has<liquid::Delete>(
      execRes.entitiesToSave.at(0)));
}

TEST_P(SpawnSpriteAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSpawnedEntityIsSelected) {
  liquid::AssetData<liquid::TextureAsset> data{};
  data.data.deviceHandle = liquid::rhi::TextureHandle{25};
  auto textureAsset = state.assetRegistry.getTextures().addAsset(data);

  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  state.camera = camera;

  liquid::editor::SpawnSpriteAtView action(textureAsset, camera);

  auto res = action.onExecute(state);
  state.selectedEntity = res.entitiesToSave.at(0);

  action.onUndo(state);

  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(
    SpawnSpriteAtViewActionTest,
    UndoSetsSelectedEntityToNullIfSelectedEntityIsADescendantOfSpawnedEntity) {
  liquid::AssetData<liquid::TextureAsset> data{};
  data.data.deviceHandle = liquid::rhi::TextureHandle{25};
  auto textureAsset = state.assetRegistry.getTextures().addAsset(data);

  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  state.camera = camera;

  liquid::editor::SpawnSpriteAtView action(textureAsset, camera);
  auto res = action.onExecute(state);
  state.selectedEntity = res.entitiesToSave.at(0);

  auto entity = res.entitiesToSave.at(0);

  {
    auto e1 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<liquid::Parent>(e1, {entity});

    auto e2 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<liquid::Parent>(e2, {e1});
    state.selectedEntity = e2;
  }

  action.onUndo(state);

  EXPECT_EQ(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(SpawnSpriteAtViewActionTest,
       UndoDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotSpawnedEntity) {
  liquid::AssetData<liquid::TextureAsset> data{};
  data.data.deviceHandle = liquid::rhi::TextureHandle{25};
  auto textureAsset = state.assetRegistry.getTextures().addAsset(data);

  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  state.camera = camera;
  state.selectedEntity = activeScene().entityDatabase.create();

  liquid::editor::SpawnSpriteAtView action(textureAsset, camera);

  action.onExecute(state);
  action.onUndo(state);

  EXPECT_NE(state.selectedEntity, liquid::Entity::Null);
}

TEST_P(SpawnSpriteAtViewActionTest,
       PredicateReturnsTrueIfCameraEntityHasCameraAndAssetExists) {
  auto textureAsset = state.assetRegistry.getTextures().addAsset({});

  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  state.camera = camera;

  liquid::editor::SpawnSpriteAtView action(textureAsset, camera);

  EXPECT_TRUE(action.predicate(state));
}

TEST_P(SpawnSpriteAtViewActionTest,
       PredicateReturnsFalseIfCameraEntityHasNoCamera) {
  auto textureAsset = state.assetRegistry.getTextures().addAsset({});

  auto camera = activeScene().entityDatabase.create();
  state.camera = camera;

  liquid::editor::SpawnSpriteAtView action(textureAsset, camera);

  EXPECT_FALSE(action.predicate(state));
}

TEST_P(SpawnSpriteAtViewActionTest, PredicateReturnsFalseIfAssetDoesNotExist) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<liquid::Camera>(camera, {});
  state.camera = camera;

  liquid::editor::SpawnSpriteAtView action(liquid::TextureAssetHandle{45},
                                           camera);

  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, SpawnSpriteAtViewActionTest);
