#include "quoll/core/Base.h"
#include "quoll/editor/actions/SpawnEntityActions.h"

#include "quoll/editor-tests/Testing.h"
#include "ActionTestBase.h"

using SpawnEmptyEntityAtViewActionTest = ActionTestBase;

TEST_P(SpawnEmptyEntityAtViewActionTest, ExecutorSpawnsEmptyEntityAtView) {
  auto camera = activeScene().entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  quoll::Camera component{};
  component.viewMatrix = viewMatrix;
  activeScene().entityDatabase.set(camera, component);
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  auto res = action.onExecute(state, assetRegistry);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_NE(res.entitiesToSave.at(0), quoll::Entity::Null);

  auto entity = res.entitiesToSave.at(0);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<quoll::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Name>(entity).name,
            "New entity");
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::WorldTransform>(entity));
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SpawnEmptyEntityAtViewActionTest, UndoRemovesSpawnedEntity) {
  auto camera = activeScene().entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  quoll::Camera component{};
  component.viewMatrix = viewMatrix;
  activeScene().entityDatabase.set(camera, component);
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  auto execRes = action.onExecute(state, assetRegistry);
  auto undoRes = action.onUndo(state, assetRegistry);

  EXPECT_EQ(execRes.entitiesToSave.size(), 1);
  EXPECT_EQ(undoRes.entitiesToDelete.size(), 1);
  EXPECT_EQ(execRes.entitiesToSave.at(0), undoRes.entitiesToDelete.at(0));
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Delete>(
      execRes.entitiesToSave.at(0)));
}

TEST_P(SpawnEmptyEntityAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSpawnedEntityIsSelected) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  auto res = action.onExecute(state, assetRegistry);
  state.selectedEntity = res.entitiesToSave.at(0);

  action.onUndo(state, assetRegistry);

  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(
    SpawnEmptyEntityAtViewActionTest,
    UndoSetsSelectedEntityToNullIfSelectedEntityIsADescendantOfSpawnedEntity) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  auto res = action.onExecute(state, assetRegistry);

  auto entity = res.entitiesToSave.at(0);

  {
    auto e1 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e1, {entity});

    auto e2 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e2, {e1});
    state.selectedEntity = e2;
  }

  action.onUndo(state, assetRegistry);

  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(SpawnEmptyEntityAtViewActionTest,
       UndoDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotSpawnedEntity) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;
  state.selectedEntity = activeScene().entityDatabase.create();

  quoll::editor::SpawnEmptyEntityAtView action;
  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  EXPECT_NE(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(SpawnEmptyEntityAtViewActionTest,
       PredicateReturnsTrueIfCameraEntityHasCamera) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_P(SpawnEmptyEntityAtViewActionTest,
       PredicateReturnsFalseIfCameraEntityHasNoCamera) {
  auto camera = activeScene().entityDatabase.create();
  state.camera = camera;

  quoll::editor::SpawnEmptyEntityAtView action;
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, SpawnEmptyEntityAtViewActionTest);

using SpawnPrefabAtViewActionTest = ActionTestBase;

TEST_P(SpawnPrefabAtViewActionTest, ExecutorSpawnsPrefabAtView) {
  auto camera = activeScene().entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  quoll::Camera component{};
  component.viewMatrix = viewMatrix;
  activeScene().entityDatabase.set(camera, component);

  quoll::AssetData<quoll::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  asset.data.transforms.push_back({1});

  auto prefab = assetRegistry.getPrefabs().addAsset(asset);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state, assetRegistry);

  ASSERT_EQ(res.entitiesToSave.size(), 3);
  EXPECT_NE(res.entitiesToSave.at(0), quoll::Entity::Null);
  EXPECT_NE(res.entitiesToSave.at(1), quoll::Entity::Null);
  EXPECT_NE(res.entitiesToSave.at(2), quoll::Entity::Null);

  auto entity = res.entitiesToSave.at(2);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<quoll::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));
  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SpawnPrefabAtViewActionTest, UndoRemovesRootNode) {
  auto camera = activeScene().entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  quoll::Camera component{};
  component.viewMatrix = viewMatrix;
  activeScene().entityDatabase.set(camera, component);

  quoll::AssetData<quoll::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  asset.data.transforms.push_back({1});

  auto prefab = assetRegistry.getPrefabs().addAsset(asset);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  auto execRes = action.onExecute(state, assetRegistry);

  auto undoRes = action.onUndo(state, assetRegistry);

  ASSERT_EQ(execRes.entitiesToSave.size(), 3);
  ASSERT_EQ(undoRes.entitiesToDelete.size(), 1);

  auto entity = undoRes.entitiesToDelete.at(0);

  EXPECT_EQ(undoRes.entitiesToDelete.back(), entity);

  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Delete>(entity));
}

TEST_P(SpawnPrefabAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSpawnedRootIsSelected) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});

  quoll::AssetData<quoll::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  auto prefab = assetRegistry.getPrefabs().addAsset(asset);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state, assetRegistry);

  auto entity = res.entitiesToSave.at(0);
  state.selectedEntity = entity;

  action.onUndo(state, assetRegistry);
  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(SpawnPrefabAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSelectedEntityIsDescendantOfSpawnedRoot) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});

  quoll::AssetData<quoll::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  asset.data.transforms.push_back({1});
  auto prefab = assetRegistry.getPrefabs().addAsset(asset);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state, assetRegistry);

  state.selectedEntity = res.entitiesToSave.back();

  action.onUndo(state, assetRegistry);
  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(
    SpawnEmptyEntityAtViewActionTest,
    UndoDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotInSpawnedPrefabTree) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});

  quoll::AssetData<quoll::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  asset.data.transforms.push_back({1});
  auto prefab = assetRegistry.getPrefabs().addAsset(asset);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state, assetRegistry);

  state.selectedEntity = quoll::Entity{25};

  action.onUndo(state, assetRegistry);
  EXPECT_NE(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(
    SpawnPrefabAtViewActionTest,
    PredicateReturnsTrueIfPrefabAssetExistsAndIsNotEmptyAndCameraEntityHasCamera) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});

  quoll::AssetData<quoll::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  auto prefab = assetRegistry.getPrefabs().addAsset(asset);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_P(SpawnPrefabAtViewActionTest,
       PredicateReturnsFalseIfPrefabAssetDoesNotExist) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  quoll::editor::SpawnPrefabAtView action(quoll::PrefabAssetHandle{15}, camera);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(SpawnPrefabAtViewActionTest, PredicateReturnsFalseIfPrefabAssetIsEmpty) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  quoll::AssetData<quoll::PrefabAsset> asset{};
  auto prefab = assetRegistry.getPrefabs().addAsset(asset);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(SpawnPrefabAtViewActionTest,
       PredicateReturnsFalseIfCameraEntityHasNoCamera) {
  auto camera = activeScene().entityDatabase.create();

  quoll::AssetData<quoll::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  auto prefab = assetRegistry.getPrefabs().addAsset(asset);

  quoll::editor::SpawnPrefabAtView action(prefab, camera);
  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, SpawnPrefabAtViewActionTest);

using SpawnSpriteAtViewActionTest = ActionTestBase;

TEST_P(SpawnSpriteAtViewActionTest, ExecutorSpawnsSpriteAtView) {
  quoll::AssetData<quoll::TextureAsset> data{};
  data.name = "my-texture";
  data.data.deviceHandle = quoll::rhi::TextureHandle{25};
  auto textureAsset = assetRegistry.getTextures().addAsset(data);

  auto camera = activeScene().entityDatabase.create();
  glm::mat4 viewMatrix =
      glm::lookAt(glm::vec3{0.0f, 0.0f, -20.0f}, glm::vec3{0.0f, 0.0f, 0.0f},
                  glm::vec3{0.0f, 1.0f, 0.0f});

  quoll::Camera component{};
  component.viewMatrix = viewMatrix;
  activeScene().entityDatabase.set(camera, component);
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(textureAsset, camera);
  auto res = action.onExecute(state, assetRegistry);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_NE(res.entitiesToSave.at(0), quoll::Entity::Null);

  auto entity = res.entitiesToSave.at(0);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<quoll::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));
  EXPECT_EQ(activeScene().entityDatabase.get<quoll::Name>(entity).name,
            "my-texture");
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::WorldTransform>(entity));
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Sprite>(entity));

  EXPECT_TRUE(res.addToHistory);
}

TEST_P(SpawnSpriteAtViewActionTest, UndoRemovesSpawnedEntity) {
  quoll::AssetData<quoll::TextureAsset> data{};
  data.data.deviceHandle = quoll::rhi::TextureHandle{25};
  auto textureAsset = assetRegistry.getTextures().addAsset(data);

  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(textureAsset, camera);
  auto execRes = action.onExecute(state, assetRegistry);
  auto undoRes = action.onUndo(state, assetRegistry);

  EXPECT_EQ(execRes.entitiesToSave.size(), 1);
  EXPECT_EQ(undoRes.entitiesToDelete.size(), 1);
  EXPECT_EQ(execRes.entitiesToSave.at(0), undoRes.entitiesToDelete.at(0));
  EXPECT_TRUE(activeScene().entityDatabase.has<quoll::Delete>(
      execRes.entitiesToSave.at(0)));
}

TEST_P(SpawnSpriteAtViewActionTest,
       UndoSetsSelectedEntityToNullIfSpawnedEntityIsSelected) {
  quoll::AssetData<quoll::TextureAsset> data{};
  data.data.deviceHandle = quoll::rhi::TextureHandle{25};
  auto textureAsset = assetRegistry.getTextures().addAsset(data);

  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(textureAsset, camera);

  auto res = action.onExecute(state, assetRegistry);
  state.selectedEntity = res.entitiesToSave.at(0);

  action.onUndo(state, assetRegistry);

  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(
    SpawnSpriteAtViewActionTest,
    UndoSetsSelectedEntityToNullIfSelectedEntityIsADescendantOfSpawnedEntity) {
  quoll::AssetData<quoll::TextureAsset> data{};
  data.data.deviceHandle = quoll::rhi::TextureHandle{25};
  auto textureAsset = assetRegistry.getTextures().addAsset(data);

  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(textureAsset, camera);
  auto res = action.onExecute(state, assetRegistry);
  state.selectedEntity = res.entitiesToSave.at(0);

  auto entity = res.entitiesToSave.at(0);

  {
    auto e1 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e1, {entity});

    auto e2 = activeScene().entityDatabase.create();
    activeScene().entityDatabase.set<quoll::Parent>(e2, {e1});
    state.selectedEntity = e2;
  }

  action.onUndo(state, assetRegistry);

  EXPECT_EQ(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(SpawnSpriteAtViewActionTest,
       UndoDoesNotSetSelectedEntityToNullIfSelectedEntityIsNotSpawnedEntity) {
  quoll::AssetData<quoll::TextureAsset> data{};
  data.data.deviceHandle = quoll::rhi::TextureHandle{25};
  auto textureAsset = assetRegistry.getTextures().addAsset(data);

  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;
  state.selectedEntity = activeScene().entityDatabase.create();

  quoll::editor::SpawnSpriteAtView action(textureAsset, camera);

  action.onExecute(state, assetRegistry);
  action.onUndo(state, assetRegistry);

  EXPECT_NE(state.selectedEntity, quoll::Entity::Null);
}

TEST_P(SpawnSpriteAtViewActionTest,
       PredicateReturnsTrueIfCameraEntityHasCameraAndAssetExists) {
  auto textureAsset = assetRegistry.getTextures().addAsset({});

  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(textureAsset, camera);

  EXPECT_TRUE(action.predicate(state, assetRegistry));
}

TEST_P(SpawnSpriteAtViewActionTest,
       PredicateReturnsFalseIfCameraEntityHasNoCamera) {
  auto textureAsset = assetRegistry.getTextures().addAsset({});

  auto camera = activeScene().entityDatabase.create();
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(textureAsset, camera);

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

TEST_P(SpawnSpriteAtViewActionTest, PredicateReturnsFalseIfAssetDoesNotExist) {
  auto camera = activeScene().entityDatabase.create();
  activeScene().entityDatabase.set<quoll::Camera>(camera, {});
  state.camera = camera;

  quoll::editor::SpawnSpriteAtView action(quoll::TextureAssetHandle{45},
                                          camera);

  EXPECT_FALSE(action.predicate(state, assetRegistry));
}

InitActionsTestSuite(EntityActionsTest, SpawnSpriteAtViewActionTest);
