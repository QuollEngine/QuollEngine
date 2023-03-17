#include "liquid/core/Base.h"
#include "liquidator/actions/SpawnEntityActions.h"

#include "liquidator-tests/Testing.h"
#include "ActionTestBase.h"

using SpawnPrefabAtTransform = ActionTestBase;

TEST_P(SpawnPrefabAtTransform, ExecutorCreatesEntitiesFromPrefab) {
  liquid::AssetData<liquid::PrefabAsset> asset{};

  // Create 5 transforms
  for (int32_t i = 0; i < 5; ++i) {
    glm::vec3 position(static_cast<float>(i));
    glm::quat rotation(static_cast<float>(i) / 5.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 scale = position;

    liquid::PrefabComponent<liquid::PrefabTransformData> transform{};
    transform.entity = i;
    transform.value.position = position;
    transform.value.rotation = rotation;
    transform.value.scale = scale;
    transform.value.parent = i - 1;
    asset.data.transforms.push_back(transform);
  }

  // Create two meshes
  for (uint32_t i = 0; i < 2; ++i) {
    liquid::PrefabComponent<liquid::MeshAssetHandle> mesh{};
    mesh.entity = i;
    mesh.value = liquid::MeshAssetHandle{i};
    asset.data.meshes.push_back(mesh);
  }

  // Create three skinned meshes
  for (uint32_t i = 2; i < 5; ++i) {
    liquid::PrefabComponent<liquid::SkinnedMeshAssetHandle> mesh{};
    mesh.entity = i;
    mesh.value = liquid::SkinnedMeshAssetHandle{i};
    asset.data.skinnedMeshes.push_back(mesh);
  }

  // create skeletons for skinned meshes
  for (uint32_t i = 2; i < 5; ++i) {
    liquid::PrefabComponent<liquid::SkeletonAssetHandle> skeleton{};
    skeleton.entity = i;
    skeleton.value = state.assetRegistry.getSkeletons().addAsset({});
    asset.data.skeletons.push_back(skeleton);
  }

  // create animators for skinned meshes
  // also create one additional animator
  for (uint32_t i = 2; i < 5; ++i) {
    liquid::PrefabComponent<liquid::Animator> animator{};
    animator.entity = i;
    animator.value.currentAnimation = i;
    asset.data.animators.push_back(animator);
  }

  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};
  liquid::editor::SpawnPrefabAtTransform action(prefab, transform);

  auto res = action.onExecute(state);
  EXPECT_EQ(res.entitiesToSave.size(), 5);

  auto &db = activeScene().entityDatabase;

  // Test relations
  // First item has no parent
  EXPECT_FALSE(db.has<liquid::Parent>(res.entitiesToSave.at(0)));
  EXPECT_EQ(db.get<liquid::Children>(res.entitiesToSave.at(0)).children.at(0),
            res.entitiesToSave.at(1));

  for (uint32_t i = 1; i < 5; ++i) {
    auto current = res.entitiesToSave.at(i);
    auto parent = res.entitiesToSave.at(i - 1);
    // All transform items have parent that is the previous item
    EXPECT_EQ(db.get<liquid::Parent>(current).parent, parent);
    EXPECT_EQ(db.get<liquid::Children>(parent).children.at(0), current);
  }

  // Test transforms

  // First item becomes the root node
  // if it is the only root node
  EXPECT_EQ(
      db.get<liquid::LocalTransform>(res.entitiesToSave.at(0)).localPosition,
      transform.localPosition);
  EXPECT_TRUE(db.has<liquid::WorldTransform>(res.entitiesToSave.at(0)));

  for (uint32_t i = 1; i < 5; ++i) {
    auto entity = res.entitiesToSave.at(i);

    const auto &transform = db.get<liquid::LocalTransform>(entity);
    const auto &pTransform = asset.data.transforms.at(i).value;
    EXPECT_EQ(transform.localPosition, pTransform.position);
    EXPECT_EQ(transform.localRotation, pTransform.rotation);
    EXPECT_EQ(transform.localScale, pTransform.scale);
    EXPECT_TRUE(db.has<liquid::WorldTransform>(entity));
  }

  // Test meshes
  for (uint32_t i = 0; i < 2; ++i) {
    auto entity = res.entitiesToSave.at(i);

    const auto &mesh = db.get<liquid::Mesh>(entity);
    EXPECT_EQ(mesh.handle, static_cast<liquid::MeshAssetHandle>(i));
  }

  // Test skinned meshes
  for (uint32_t i = 2; i < 5; ++i) {
    auto entity = res.entitiesToSave.at(i);

    const auto &mesh = db.get<liquid::SkinnedMesh>(entity);
    EXPECT_EQ(mesh.handle, static_cast<liquid::SkinnedMeshAssetHandle>(i));
  }

  // Test skeletons
  for (uint32_t i = 2; i < 5; ++i) {
    auto entity = res.entitiesToSave.at(i);

    const auto &skeleton = db.get<liquid::Skeleton>(entity);

    // Skeletons vector only has three items
    EXPECT_EQ(skeleton.assetHandle, asset.data.skeletons.at(i - 2).value);
  }

  // Test animators
  for (uint32_t i = 2; i < 5; ++i) {
    auto entity = res.entitiesToSave.at(i);
    const auto &animator = db.get<liquid::Animator>(entity);
    EXPECT_EQ(animator.currentAnimation, i);
  }
}

TEST_P(SpawnPrefabAtTransform, ExecutorCreatesParentBeforeChild) {
  liquid::AssetData<liquid::PrefabAsset> asset{};

  {
    liquid::PrefabComponent<liquid::PrefabTransformData> transform{};
    transform.entity = 0;
    transform.value.parent = 1;
    transform.value.position = glm::vec3(0.2f);
    asset.data.transforms.push_back(transform);
  }
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};
  liquid::editor::SpawnPrefabAtTransform action(prefab, transform);
  auto res = action.onExecute(state);
  EXPECT_EQ(res.entitiesToSave.size(), 2);

  auto &db = activeScene().entityDatabase;

  auto root = res.entitiesToSave.at(0);
  auto child = res.entitiesToSave.at(1);

  EXPECT_EQ(db.get<liquid::LocalTransform>(root).localPosition,
            glm::vec3(0.5f));
  EXPECT_EQ(db.get<liquid::LocalTransform>(child).localPosition,
            glm::vec3(0.2f));
}

TEST_P(
    SpawnPrefabAtTransform,
    ExecutorWrapsAllSpawnedEntitiesInAParentIfPrefabHasMoreThanOneRootEntity) {
  liquid::AssetData<liquid::PrefabAsset> asset{};

  {
    liquid::PrefabComponent<liquid::PrefabTransformData> transform{};
    transform.entity = 0;
    transform.value.parent = -1;
    asset.data.transforms.push_back(transform);

    liquid::PrefabComponent<liquid::MeshAssetHandle> mesh{};
    mesh.entity = 1;
    asset.data.meshes.push_back(mesh);
  }
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};
  liquid::editor::SpawnPrefabAtTransform action(prefab, transform);
  auto res = action.onExecute(state);
  EXPECT_EQ(res.entitiesToSave.size(), 3);

  auto &db = activeScene().entityDatabase;

  auto root = res.entitiesToSave.at(res.entitiesToSave.size() - 1);
  EXPECT_EQ(db.get<liquid::LocalTransform>(root).localPosition,
            transform.localPosition);
  EXPECT_TRUE(db.has<liquid::WorldTransform>(root));

  EXPECT_FALSE(activeScene().entityDatabase.has<liquid::Parent>(root));

  auto children = db.get<liquid::Children>(root).children;
  EXPECT_EQ(children.size(), 2);
  for (size_t i = 0; i < children.size(); ++i) {
    auto entity = res.entitiesToSave.at(i);
    EXPECT_EQ(children.at(i), entity);
    EXPECT_EQ(db.get<liquid::Parent>(entity).parent, root);
  }
}

TEST_P(SpawnPrefabAtTransform,
       ExecutorSpawnsSingleEntityIfPrefabHasOneRootEntity) {
  liquid::AssetData<liquid::PrefabAsset> asset{};
  for (int32_t i = 0; i < 2; ++i) {
    glm::vec3 position(static_cast<float>(i));
    glm::quat rotation(static_cast<float>(i) / 5.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 scale = position;

    liquid::PrefabComponent<liquid::PrefabTransformData> transform{};
    transform.entity = i;
    transform.value.position = position;
    transform.value.rotation = rotation;
    transform.value.scale = scale;
    transform.value.parent = i - 1;
    asset.data.transforms.push_back(transform);
  }
  auto prefab = state.assetRegistry.getPrefabs().addAsset(asset);

  liquid::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};
  liquid::editor::SpawnPrefabAtTransform action(prefab, transform);
  auto res = action.onExecute(state);
  EXPECT_EQ(res.entitiesToSave.size(), 2);

  auto &db = activeScene().entityDatabase;

  auto root = res.entitiesToSave.at(0);

  EXPECT_EQ(db.get<liquid::LocalTransform>(root).localPosition,
            transform.localPosition);
  EXPECT_TRUE(db.has<liquid::WorldTransform>(root));
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
