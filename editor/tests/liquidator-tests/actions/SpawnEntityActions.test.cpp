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

using SpawnPrefabAtTransform = ActionTestBase;

TEST_P(SpawnPrefabAtTransform, ExecutorCreatesEntitiesFromPrefab) {
  liquid::AssetData<liquid::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});

  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};
  liquid::editor::SpawnPrefabAtTransform action(prefab, transform);

  auto res = action.onExecute(state);
  EXPECT_EQ(res.entitiesToSave.size(), 1);

  auto entity = res.entitiesToSave.at(0);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            transform.localPosition);
}

TEST_P(SpawnPrefabAtTransform,
       PredicateReturnsTrueIfPrefabAssetExistsAndIsNotEmpty) {
  liquid::AssetData<liquid::PrefabAsset> asset{};
  asset.data.transforms.push_back({0});
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtTransform action(prefab, {});
  EXPECT_TRUE(action.predicate(state));
}

TEST_P(SpawnPrefabAtTransform, PredicateReturnsFalseIfPrefabAssetDoesNotExist) {
  liquid::editor::SpawnPrefabAtTransform action(liquid::PrefabAssetHandle{15},
                                                {});
  EXPECT_FALSE(action.predicate(state));
}

TEST_P(SpawnPrefabAtTransform, PredicateReturnsFalseIfPrefabAssetIsEmpty) {
  liquid::AssetData<liquid::PrefabAsset> asset{};
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtTransform action(prefab, {});
  EXPECT_FALSE(action.predicate(state));
}

InitActionsTestSuite(EntityActionsTest, SpawnPrefabAtTransform);

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
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::editor::SpawnPrefabAtView action(prefab, camera);
  auto res = action.onExecute(state);

  ASSERT_EQ(res.entitiesToSave.size(), 1);
  EXPECT_NE(res.entitiesToSave.at(0), liquid::Entity::Null);

  auto entity = res.entitiesToSave.at(0);

  EXPECT_EQ(activeScene()
                .entityDatabase.get<liquid::LocalTransform>(entity)
                .localPosition,
            glm::vec3(0.0f, 0.0f, -10.0f));
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
