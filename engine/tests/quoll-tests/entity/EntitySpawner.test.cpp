#include "quoll/core/Base.h"
#include "quoll/core/Name.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/entity/EntitySpawner.h"
#include "quoll/renderer/Mesh.h"
#include "quoll/scene/Children.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll/scene/Parent.h"
#include "quoll/scene/Sprite.h"
#include "quoll/scene/WorldTransform.h"
#include "quoll/skeleton/Skeleton.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheUtils.h"

class EntitySpawnerTest : public ::testing::Test {
public:
  EntitySpawnerTest()
      : entitySpawner(entityDatabase, assetCache), assetCache("/") {}

  template <typename TAssetData>
  quoll::AssetRef<TAssetData> createAsset(TAssetData data = {},
                                          quoll::String name = "") {
    return createAssetInCache(assetCache, data, name);
  }

  quoll::AssetCache assetCache;
  quoll::EntityDatabase entityDatabase;
  quoll::EntitySpawner entitySpawner;
};

using EntitySpawnerDeathTest = EntitySpawnerTest;

TEST_F(EntitySpawnerTest, SpawnEmptyCreatesEmptyEntity) {
  quoll::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};

  auto entity = entitySpawner.spawnEmpty(transform);
  EXPECT_TRUE(entityDatabase.exists(entity));
  EXPECT_TRUE(entityDatabase.has<quoll::LocalTransform>(entity));
  EXPECT_TRUE(entityDatabase.has<quoll::WorldTransform>(entity));
  EXPECT_TRUE(entityDatabase.has<quoll::Name>(entity));

  EXPECT_EQ(entityDatabase.get<quoll::LocalTransform>(entity).localPosition,
            transform.localPosition);
}

TEST_F(EntitySpawnerDeathTest, SpawnPrefabFailsIfPrefabDoesNotExist) {
  EXPECT_DEATH(
      entitySpawner.spawnPrefab(quoll::AssetRef<quoll::PrefabAsset>(), {}),
      ".*");
}

TEST_F(EntitySpawnerDeathTest, SpawnPrefabReturnsEmptyListIfPrefabIsEmpty) {
  auto prefab = createAsset<quoll::PrefabAsset>();
  EXPECT_DEATH(entitySpawner.spawnPrefab(prefab, {}), ".*");
}

TEST_F(EntitySpawnerTest, SpawnPrefabCreatesEntitiesFromPrefab) {
  quoll::AssetData<quoll::PrefabAsset> asset{};

  // Create 1 transforms with no parent
  {
    glm::vec3 position(0.0f);
    glm::quat rotation(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 scale = position;

    quoll::PrefabComponent<quoll::PrefabTransformData> transform{};
    transform.entity = 0;
    transform.value.position = position;
    transform.value.rotation = rotation;
    transform.value.scale = scale;
    transform.value.parent = -1;
    asset.data.transforms.push_back(transform);
  }

  // Create 2 transforms that point to previous entity
  for (i32 i = 1; i < 3; ++i) {
    glm::vec3 position(static_cast<f32>(i));
    glm::quat rotation(static_cast<f32>(i) / 5.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 scale = position;

    quoll::PrefabComponent<quoll::PrefabTransformData> transform{};
    transform.entity = i;
    transform.value.position = position;
    transform.value.rotation = rotation;
    transform.value.scale = scale;
    transform.value.parent = 0;
    asset.data.transforms.push_back(transform);
  }

  // Create transforms that point to previous parent
  for (i32 i = 3; i < 5; ++i) {
    glm::vec3 position(static_cast<f32>(i));
    glm::quat rotation(static_cast<f32>(i) / 5.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 scale = position;

    quoll::PrefabComponent<quoll::PrefabTransformData> transform{};
    transform.entity = i;
    transform.value.position = position;
    transform.value.rotation = rotation;
    transform.value.scale = scale;
    transform.value.parent = i - 1;
    asset.data.transforms.push_back(transform);
  }

  // Create two meshes
  for (u32 i = 0; i < 2; ++i) {
    quoll::PrefabComponent<quoll::AssetRef<quoll::MeshAsset>> mesh{};
    mesh.entity = i;
    mesh.value = createAsset<quoll::MeshAsset>();
    asset.data.meshes.push_back(mesh);
  }

  std::vector<quoll::AssetRef<quoll::MaterialAsset>> materials;
  for (u32 i = 0; i < 3; ++i) {
    auto material = createAsset<quoll::MaterialAsset>();
    materials.push_back(material);
  }

  // Create two mesh renderers with 3 materials each
  for (u32 i = 0; i < 2; ++i) {
    quoll::PrefabComponent<quoll::MeshRenderer> renderer{};
    renderer.entity = i;
    renderer.value.materials.push_back(materials.at(0));
    renderer.value.materials.push_back(materials.at(1));
    renderer.value.materials.push_back(materials.at(2));
    asset.data.meshRenderers.push_back(renderer);
  }

  // Create three skinned mesh renderers with 2 materials each
  for (u32 i = 0; i < 3; ++i) {
    quoll::PrefabComponent<quoll::SkinnedMeshRenderer> renderer{};
    renderer.entity = i;
    renderer.value.materials.push_back(materials.at(0));
    renderer.value.materials.push_back(materials.at(1));
    asset.data.skinnedMeshRenderers.push_back(renderer);
  }

  // Create names
  for (u32 i = 3; i < 5; ++i) {
    quoll::PrefabComponent<quoll::String> name{};
    name.entity = i;
    name.value = "Test name " + std::to_string(i);
    asset.data.names.push_back(name);
  }

  // Create three skinned meshes
  for (u32 i = 2; i < 5; ++i) {
    quoll::PrefabComponent<quoll::AssetRef<quoll::MeshAsset>> mesh{};
    mesh.entity = i;
    mesh.value = createAsset<quoll::MeshAsset>();
    asset.data.meshes.push_back(mesh);
  }

  // create skeletons for skinned meshes
  for (u32 i = 2; i < 5; ++i) {
    quoll::PrefabComponent<quoll::AssetRef<quoll::SkeletonAsset>> skeleton{};
    skeleton.entity = i;
    skeleton.value = createAsset<quoll::SkeletonAsset>();
    asset.data.skeletons.push_back(skeleton);
  }

  for (u32 i = 2; i < 5; ++i) {
    asset.data.animations.push_back(createAsset<quoll::AnimationAsset>());
  }

  // create animators for skinned meshes
  // also create one additional animator
  for (u32 i = 2; i < 5; ++i) {
    quoll::PrefabComponent<quoll::AssetRef<quoll::AnimatorAsset>> animator{};
    animator.entity = i;
    animator.value = createAsset<quoll::AnimatorAsset>({.initialState = i});
    asset.data.animators.push_back(animator);
  }

  // Create two directional lights
  for (u32 i = 1; i < 3; ++i) {
    quoll::PrefabComponent<quoll::DirectionalLight> light{};
    light.entity = i;
    light.value.intensity = 25.0f;
    asset.data.directionalLights.push_back(light);
  }

  // Create two point lights
  for (u32 i = 3; i < 5; ++i) {
    quoll::PrefabComponent<quoll::PointLight> light{};
    light.entity = i;
    light.value.range = 25.0f;
    asset.data.pointLights.push_back(light);
  }

  auto prefab = createAsset<quoll::PrefabAsset>(asset.data);

  quoll::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};
  auto res = entitySpawner.spawnPrefab(prefab, transform);

  EXPECT_EQ(res.size(), 5);

  auto &db = entityDatabase;

  // Test relations
  // First item has no parent
  EXPECT_FALSE(db.has<quoll::Parent>(res.at(0)));
  EXPECT_EQ(db.get<quoll::Children>(res.at(0)).children.size(), 2);
  EXPECT_EQ(db.get<quoll::Children>(res.at(0)).children.at(0), res.at(1));
  EXPECT_EQ(db.get<quoll::Children>(res.at(0)).children.at(1), res.at(2));

  // Second and third items have first entity as parent
  for (u32 i = 1; i < 3; ++i) {
    auto current = res.at(i);
    EXPECT_TRUE(db.has<quoll::Parent>(current));
    EXPECT_EQ(db.get<quoll::Parent>(current).parent, res.at(0));
  }

  for (u32 i = 3; i < 5; ++i) {
    auto current = res.at(i);
    auto parent = res.at(i - 1);
    // All transform items have parent that is the previous item
    EXPECT_EQ(db.get<quoll::Parent>(current).parent, parent);

    EXPECT_TRUE(db.has<quoll::Children>(parent));
    EXPECT_EQ(db.get<quoll::Children>(parent).children.at(0), current);
  }

  // Test transforms

  // First item becomes the root node
  // if it is the only root node
  EXPECT_EQ(db.get<quoll::LocalTransform>(res.at(0)).localPosition,
            transform.localPosition);
  EXPECT_TRUE(db.has<quoll::WorldTransform>(res.at(0)));

  for (u32 i = 1; i < 5; ++i) {
    auto entity = res.at(i);

    const auto &transform = db.get<quoll::LocalTransform>(entity);
    const auto &pTransform = asset.data.transforms.at(i).value;
    EXPECT_EQ(transform.localPosition, pTransform.position);
    EXPECT_EQ(transform.localRotation, pTransform.rotation);
    EXPECT_EQ(transform.localScale, pTransform.scale);
    EXPECT_TRUE(db.has<quoll::WorldTransform>(entity));
  }

  // Test names

  // First three items have names with Untitled
  for (u32 i = 0; i < 3; ++i) {
    auto entity = res.at(i);
    const auto &name = db.get<quoll::Name>(entity);

    EXPECT_EQ(name.name, "New entity");
  }

  for (u32 i = 3; i < 5; ++i) {
    auto entity = res.at(i);
    const auto &name = db.get<quoll::Name>(entity);

    EXPECT_EQ(name.name, "Test name " + std::to_string(i));
  }

  // Test meshes
  for (u32 i = 0; i < 2; ++i) {
    auto entity = res.at(i);

    const auto &mesh = db.get<quoll::Mesh>(entity);
    EXPECT_EQ(mesh.asset, asset.data.meshes.at(i).value);
  }

  // Test skinned meshes
  for (u32 i = 2; i < 5; ++i) {
    auto entity = res.at(i);

    const auto &mesh = db.get<quoll::Mesh>(entity);
    EXPECT_EQ(mesh.asset, asset.data.meshes.at(i).value);
  }

  // Test mesh renderers
  for (u32 i = 0; i < 2; ++i) {
    auto entity = res.at(i);
    const auto &renderer = db.get<quoll::MeshRenderer>(entity);

    for (usize mi = 0; mi < renderer.materials.size(); ++mi) {
      EXPECT_EQ(renderer.materials.at(mi).handle().getRawId(), mi + 1);
    }
  }

  // Test skinned mesh renderer
  for (u32 i = 0; i < 3; ++i) {
    auto entity = res.at(i);
    const auto &renderer = db.get<quoll::SkinnedMeshRenderer>(entity);

    for (usize mi = 0; mi < renderer.materials.size(); ++mi) {
      EXPECT_EQ(renderer.materials.at(mi).handle().getRawId(), mi + 1);
    }
  }

  // Test skeletons
  for (u32 i = 2; i < 5; ++i) {
    auto entity = res.at(i);

    const auto &skeleton = db.get<quoll::SkeletonAssetRef>(entity);

    // Skeletons vector only has three items
    EXPECT_EQ(skeleton.asset, asset.data.skeletons.at(i - 2).value);
  }

  // Test animators
  for (u32 i = 2; i < 5; ++i) {
    auto entity = res.at(i);
    const auto &animator = db.get<quoll::AnimatorAssetRef>(entity);
    EXPECT_TRUE(animator.asset);
  }

  for (u32 i = 1; i < 3; ++i) {
    auto entity = res.at(i);
    const auto &light = db.get<quoll::DirectionalLight>(entity);
    EXPECT_EQ(light.intensity, 25.0f);
  }

  for (u32 i = 3; i < 5; ++i) {
    auto entity = res.at(i);
    const auto &light = db.get<quoll::PointLight>(entity);
    EXPECT_EQ(light.range, 25.0f);
  }
}

TEST_F(EntitySpawnerTest, SpawnPrefabCreatesParentsBeforeChild) {
  quoll::PrefabAsset assetData{};

  {
    quoll::PrefabComponent<quoll::PrefabTransformData> transform{};
    transform.entity = 0;
    transform.value.parent = 1;
    transform.value.position = glm::vec3(0.2f);
    assetData.transforms.push_back(transform);
  }

  auto prefab = createAsset(assetData);

  quoll::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};
  auto res = entitySpawner.spawnPrefab(prefab, transform);
  EXPECT_EQ(res.size(), 2);

  auto &db = entityDatabase;

  auto root = res.at(0);
  auto child = res.at(1);

  EXPECT_EQ(db.get<quoll::LocalTransform>(root).localPosition, glm::vec3(0.5f));
  EXPECT_EQ(db.get<quoll::LocalTransform>(child).localPosition,
            glm::vec3(0.2f));
}

TEST_F(
    EntitySpawnerTest,
    SpawnPrefabWrapsAllSpawnedEntitiesInAParentIfPrefabHasMoreThanOneRootEntity) {
  quoll::PrefabAsset assetData{};

  {
    quoll::PrefabComponent<quoll::PrefabTransformData> transform{};
    transform.entity = 0;
    transform.value.parent = -1;
    assetData.transforms.push_back(transform);

    quoll::PrefabComponent<quoll::AssetRef<quoll::MeshAsset>> mesh{};
    mesh.entity = 1;
    mesh.value = createAsset<quoll::MeshAsset>();
    assetData.meshes.push_back(mesh);
  }
  auto prefab = createAsset(assetData, "my-prefab");

  quoll::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};
  auto res = entitySpawner.spawnPrefab(prefab, transform);
  EXPECT_EQ(res.size(), 3);

  auto &db = entityDatabase;

  auto root = res.at(res.size() - 1);
  EXPECT_EQ(db.get<quoll::LocalTransform>(root).localPosition,
            transform.localPosition);
  EXPECT_TRUE(db.has<quoll::WorldTransform>(root));
  EXPECT_EQ(db.get<quoll::Name>(root).name, "my-prefab");

  EXPECT_FALSE(entityDatabase.has<quoll::Parent>(root));

  auto children = db.get<quoll::Children>(root).children;
  EXPECT_EQ(children.size(), 2);
  for (usize i = 0; i < children.size(); ++i) {
    auto entity = res.at(i);
    EXPECT_EQ(children.at(i), entity);
    EXPECT_EQ(db.get<quoll::Parent>(entity).parent, root);
  }
}

TEST_F(EntitySpawnerTest,
       SpawnPrefabCreatesSingleEntityIfPrefabHasOneRootEntity) {
  quoll::PrefabAsset assetData{};
  for (i32 i = 0; i < 2; ++i) {
    glm::vec3 position(static_cast<f32>(i));
    glm::quat rotation(static_cast<f32>(i) / 5.0f, 0.0f, 0.0f, 1.0f);
    glm::vec3 scale = position;

    quoll::PrefabComponent<quoll::PrefabTransformData> transform{};
    transform.entity = i;
    transform.value.position = position;
    transform.value.rotation = rotation;
    transform.value.scale = scale;
    transform.value.parent = i - 1;
    assetData.transforms.push_back(transform);
  }
  auto prefab = createAsset(assetData);

  quoll::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};
  auto res = entitySpawner.spawnPrefab(prefab, transform);
  EXPECT_EQ(res.size(), 2);

  auto &db = entityDatabase;

  auto root = res.at(0);

  EXPECT_EQ(db.get<quoll::LocalTransform>(root).localPosition,
            transform.localPosition);
  EXPECT_TRUE(db.has<quoll::WorldTransform>(root));
  EXPECT_EQ(entityDatabase.get<quoll::Name>(root).name, "New entity");
}

TEST_F(EntitySpawnerTest,
       SpawnSpriteCreatesEntityWithSpriteAndTransformComponents) {
  auto asset = createAsset<quoll::TextureAsset>({}, "my-sprite");

  quoll::LocalTransform transform{glm::vec3(0.5f, 0.5f, 0.5f)};

  auto entity = entitySpawner.spawnSprite(asset, transform);
  EXPECT_TRUE(entityDatabase.exists(entity));
  EXPECT_EQ(entityDatabase.get<quoll::LocalTransform>(entity).localPosition,
            transform.localPosition);
  EXPECT_TRUE(entityDatabase.has<quoll::WorldTransform>(entity));
  EXPECT_TRUE(entityDatabase.has<quoll::Sprite>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::Sprite>(entity).texture, asset);
  EXPECT_EQ(entityDatabase.get<quoll::Name>(entity).name, "my-sprite");
}
