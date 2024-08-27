#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/AssetFileHeader.h"
#include "quoll/asset/InputBinaryStream.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include <random>

class AssetCachePrefabTest : public AssetCacheTestBase {
public:
  quoll::AssetData<quoll::PrefabAsset> createPrefabAsset() {
    quoll::AssetData<quoll::PrefabAsset> asset;
    asset.name = "test-prefab0";
    asset.uuid = quoll::Uuid::generate();

    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<f32> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<i32> idist(-1, 2);
    std::uniform_real_distribution<f32> distColor(0.0f, 1.0f);
    std::uniform_real_distribution<f32> distPositive(0.0f, 10.0f);

    u32 numMaterials = 5;
    u32 numTransforms = 5;
    u32 numMeshes = 5;
    u32 numMeshRenderers = 4;
    u32 numSkeletons = 3;
    u32 numSkinnedMeshRenderers = 2;
    u32 numAnimators = 4;
    u32 numAnimations = 3;
    u32 numDirectionalLights = 2;
    u32 numPointLights = 4;
    u32 numNames = 3;

    for (u32 i = 0; i < numTransforms; ++i) {
      quoll::PrefabTransformData transform{};
      transform.position = {dist(mt), dist(mt), dist(mt)};
      transform.rotation = {dist(mt), dist(mt), dist(mt), dist(mt)};
      transform.scale = {dist(mt), dist(mt), dist(mt)};
      transform.parent = idist(mt);

      asset.data.transforms.push_back({i, transform});
    }

    for (u32 i = 0; i < numNames; ++i) {
      quoll::String name = "Test name " + std::to_string(i);

      asset.data.names.push_back({i, name});
    }

    for (u32 i = 0; i < numDirectionalLights; ++i) {
      quoll::DirectionalLight light{};
      light.color = {distColor(mt), distColor(mt), distColor(mt),
                     distColor(mt)};
      light.intensity = distPositive(mt);

      asset.data.directionalLights.push_back({i, light});
    }

    for (u32 i = 0; i < numPointLights; ++i) {
      quoll::PointLight light{};
      light.color = {distColor(mt), distColor(mt), distColor(mt),
                     distColor(mt)};
      light.intensity = distPositive(mt);
      light.range = distPositive(mt);

      asset.data.pointLights.push_back({i, light});
    }

    std::vector<quoll::AssetHandle<quoll::MaterialAsset>> materials(
        numMaterials);
    for (u32 i = 0; i < numMaterials; ++i) {
      quoll::AssetData<quoll::MaterialAsset> material;
      material.uuid = quoll::Uuid("material-" + std::to_string(i));
      materials.at(i) = cache.getRegistry().add(material);
    }

    for (u32 i = 0; i < numMeshes; ++i) {
      quoll::AssetData<quoll::MeshAsset> mesh;
      mesh.uuid = quoll::Uuid("mesh-" + std::to_string(i));
      auto handle = cache.getRegistry().add(mesh);
      asset.data.meshes.push_back({i, handle});
    }

    for (u32 i = 0; i < numMeshRenderers; ++i) {
      quoll::MeshRenderer renderer{};
      renderer.materials.push_back(materials.at(i % 3));
      renderer.materials.push_back(materials.at(1 + (i % 3)));

      asset.data.meshRenderers.push_back({i, renderer});
    }

    // Add two more entities that point to the same
    // meshes to test that existing meshes are always
    // referenced instead of added again
    for (u32 i = 0; i < 2; ++i) {
      auto handle = asset.data.meshes.at(static_cast<usize>(i)).value;
      asset.data.meshes.push_back({numMeshes + i, handle});
    }

    for (u32 i = 0; i < numSkinnedMeshRenderers; ++i) {
      quoll::SkinnedMeshRenderer renderer{};
      renderer.materials.push_back(materials.at((i % 2)));
      renderer.materials.push_back(materials.at(1 + (i % 2)));
      renderer.materials.push_back(materials.at(2 + (i % 2)));

      asset.data.skinnedMeshRenderers.push_back({i, renderer});
    }

    for (u32 i = 0; i < numSkeletons; ++i) {
      quoll::AssetData<quoll::SkeletonAsset> skeleton;
      skeleton.uuid = quoll::Uuid("skel-" + std::to_string(i));
      auto handle = cache.getRegistry().add(skeleton);
      asset.data.skeletons.push_back({i, handle});
    }

    // Add two more entities that point to the same
    // skeletons to test that existing skeletons are always
    // referenced instead of added again
    for (u32 i = 0; i < 2; ++i) {
      auto handle = asset.data.skeletons.at(static_cast<usize>(i)).value;
      asset.data.skeletons.push_back({numSkeletons + i, handle});
    }

    for (u32 i = 0; i < numAnimations; ++i) {
      quoll::AssetData<quoll::AnimationAsset> animation;
      animation.uuid = quoll::Uuid("animation-" + std::to_string(i));

      auto handle = cache.getRegistry().add(animation);
      asset.data.animations.push_back(handle);
    }

    for (u32 i = 0; i < numAnimators; ++i) {
      quoll::AssetData<quoll::AnimatorAsset> animator;
      animator.uuid = quoll::Uuid("animator-" + std::to_string(i));
      auto handle = cache.getRegistry().add(animator);
      asset.data.animators.push_back({i, handle});
    }

    // Add two more entities that point to same animations
    // to test that existing animations are always
    // referenced instead of added again
    for (u32 i = 0; i < 2; ++i) {
      auto handle = asset.data.animators.at(i).value;
      asset.data.animators.push_back({numAnimators + i, handle});
    }

    return asset;
  }

  void SetUp() override {
    AssetCacheTestBase::SetUp();

    textureUuid = quoll::Uuid::generate();
    cache.createTextureFromSource(FixturesPath / "1x1-2d.ktx", textureUuid);
  }

  quoll::Uuid textureUuid;
};

TEST_F(AssetCachePrefabTest, CreatesMetaFileFromAsset) {
  auto asset = createPrefabAsset();
  auto filePath = cache.createPrefabFromAsset(asset);
  auto meta = cache.getAssetMeta(asset.uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Prefab);
  EXPECT_EQ(meta.name, "test-prefab0");
}

TEST_F(AssetCachePrefabTest, CreatesPrefabFile) {
  auto asset = createPrefabAsset();

  auto filePath = cache.createPrefabFromAsset(asset);
  quoll::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  quoll::AssetFileHeader header;
  file.read(header);
  quoll::String magic(quoll::AssetFileMagicLength, '$');
  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.type, quoll::AssetType::Prefab);

  std::vector<quoll::Uuid> actualMaterials;
  {
    auto &actual = actualMaterials;
    u32 numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, 4);
    actualMaterials.resize(4);
    file.read(actual);

    for (u32 i = 0; i < numAssets; ++i) {
      auto handle = cache.getRegistry().findHandleByUuid<quoll::MaterialAsset>(
          actual.at(i));
      EXPECT_NE(handle, quoll::AssetHandle<quoll::MaterialAsset>());
    }
  }

  std::vector<quoll::Uuid> actualMeshes;
  {
    auto &expected = asset.data.meshes;
    u32 numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, static_cast<u32>(expected.size() - 2));
    actualMeshes.resize(numAssets);
    file.read(actualMeshes);

    for (u32 i = 0; i < numAssets; ++i) {
      auto expectedString =
          cache.getRegistry().getMeta(expected.at(i).value).uuid;
      EXPECT_EQ(expectedString, actualMeshes.at(i));
    }
  }

  std::vector<quoll::Uuid> actualSkeletons;
  {
    auto &expected = asset.data.skeletons;
    u32 numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, 3);
    actualSkeletons.resize(numAssets);
    file.read(actualSkeletons);

    for (u32 i = 0; i < numAssets; ++i) {
      auto expectedString =
          cache.getRegistry().getMeta(expected.at(i).value).uuid;
      EXPECT_EQ(expectedString, actualSkeletons.at(i));
    }
  }

  std::vector<quoll::Uuid> actualAnimations;
  {
    auto &expected = asset.data.animations;
    auto &actual = actualAnimations;
    u32 numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, 3);
    actualAnimations.resize(numAssets);
    file.read(actual);

    for (u32 i = 0; i < numAssets; ++i) {
      auto expectedString = cache.getRegistry().getMeta(expected.at(i)).uuid;
      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  std::vector<quoll::Uuid> actualAnimators;
  {
    auto &expected = asset.data.animators;
    auto &actual = actualAnimators;
    u32 numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, 4);
    actualAnimators.resize(numAssets);
    file.read(actual);

    for (u32 i = 0; i < numAssets; ++i) {
      auto expectedString =
          cache.getRegistry().getMeta(expected.at(i).value).uuid;
      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  {
    u32 numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 5);
    std::vector<quoll::PrefabTransformData> transforms(numComponents);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 0;
      file.read(entity);

      glm::vec3 position;
      glm::quat rotation;
      glm::vec3 scale;
      i32 parent = -1;
      file.read(position);
      file.read(rotation);
      file.read(scale);
      file.read(parent);

      EXPECT_EQ(asset.data.transforms.at(i).entity, entity);
      EXPECT_EQ(asset.data.transforms.at(i).value.position, position);
      EXPECT_EQ(asset.data.transforms.at(i).value.rotation, rotation);
      EXPECT_EQ(asset.data.transforms.at(i).value.scale, scale);
      EXPECT_EQ(asset.data.transforms.at(i).value.parent, parent);
    }
  }

  {
    u32 numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 3);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      quoll::String name;
      file.read(name);

      EXPECT_EQ(asset.data.names.at(i).value, name);
    }
  }

  {
    u32 numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 7);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      u32 meshIndex = 999;
      file.read(meshIndex);
      EXPECT_EQ(meshIndex, i % 5);

      auto &expected =
          cache.getRegistry().getMeta(asset.data.meshes.at(i).value);

      EXPECT_EQ(expected.uuid, actualMeshes.at(meshIndex));
    }
  }

  {
    u32 numComponents = 999;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 4);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      u32 numMaterials = 999;
      file.read(numMaterials);
      EXPECT_EQ(numMaterials, 2);

      std::vector<u32> materialIndices(numMaterials);
      file.read(materialIndices);

      auto &expected = asset.data.meshRenderers.at(i).value;
      EXPECT_EQ(materialIndices.size(), expected.materials.size());

      for (usize mi = 0; mi < materialIndices.size(); ++mi) {
        auto materialIndex = materialIndices.at(mi);
        auto handle = expected.materials.at(mi);

        auto uuid = actualMaterials.at(materialIndex);
        EXPECT_EQ(uuid, cache.getRegistry().getMeta(handle).uuid);
      }
    }
  }

  {
    u32 numComponents = 999;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 2);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      u32 numMaterials = 999;
      file.read(numMaterials);
      EXPECT_EQ(numMaterials, 3);

      std::vector<u32> materialIndices(numMaterials);
      file.read(materialIndices);

      auto &expected = asset.data.skinnedMeshRenderers.at(i).value;
      EXPECT_EQ(materialIndices.size(), expected.materials.size());

      for (usize mi = 0; mi < materialIndices.size(); ++mi) {
        auto materialIndex = materialIndices.at(mi);
        auto handle = expected.materials.at(mi);

        auto uuid = actualMaterials.at(materialIndex);
        EXPECT_EQ(uuid, cache.getRegistry().getMeta(handle).uuid);
      }
    }
  }

  {
    u32 numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 5);
    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      u32 skeletonIndex = 999;
      file.read(skeletonIndex);
      EXPECT_EQ(skeletonIndex, i % 3);

      auto &expected =
          cache.getRegistry().getMeta(asset.data.skeletons.at(i).value);

      EXPECT_EQ(expected.uuid, actualSkeletons.at(skeletonIndex));
    }
  }

  {
    u32 numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 3);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 animatorIndex = 999;
      file.read(animatorIndex);

      auto &expected = cache.getRegistry().getMeta(asset.data.animations.at(i));

      EXPECT_EQ(expected.uuid, actualAnimations.at(animatorIndex));
    }
  }

  {
    u32 numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 6);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      u32 animatorIndex = 999;
      file.read(animatorIndex);

      auto &expected =
          cache.getRegistry().getMeta(asset.data.animators.at(i).value);

      EXPECT_EQ(expected.uuid, actualAnimators.at(animatorIndex));
    }
  }

  {
    u32 numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 2);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      glm::vec4 color{999.0f};
      f32 intensity = 0.0f;
      file.read(color);
      file.read(intensity);

      EXPECT_EQ(asset.data.directionalLights.at(i).value.color, color);
      EXPECT_EQ(asset.data.directionalLights.at(i).value.intensity, intensity);
    }
  }

  {
    u32 numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 4);

    for (u32 i = 0; i < numComponents; ++i) {
      u32 entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      glm::vec4 color{999.0f};
      f32 intensity = 0.0f;
      f32 range = 99.0f;
      file.read(color);
      file.read(intensity);
      file.read(range);

      EXPECT_EQ(asset.data.pointLights.at(i).value.color, color);
      EXPECT_EQ(asset.data.pointLights.at(i).value.intensity, intensity);
      EXPECT_EQ(asset.data.pointLights.at(i).value.range, range);
    }
  }
}

TEST_F(AssetCachePrefabTest, FailsLoadingPrefabIfPrefabHasNoComponents) {
  quoll::AssetData<quoll::PrefabAsset> asset;
  asset.uuid = quoll::Uuid::generate();

  auto filePath = cache.createPrefabFromAsset(asset);

  auto res = cache.loadPrefab(asset.uuid);
  EXPECT_TRUE(res.hasError());
  EXPECT_FALSE(res.hasData());
  EXPECT_FALSE(res.hasWarnings());
}

TEST_F(AssetCachePrefabTest, LoadsPrefabFile) {
  auto asset = createPrefabAsset();
  auto filePath = cache.createPrefabFromAsset(asset);
  auto handle = cache.loadPrefab(asset.uuid);
  EXPECT_NE(handle.getData(), quoll::AssetHandle<quoll::PrefabAsset>());
  EXPECT_FALSE(handle.hasWarnings());

  auto &prefab = cache.getRegistry().get(handle.getData());
  EXPECT_EQ(cache.getRegistry().getMeta(handle.getData()).name, asset.name);

  EXPECT_EQ(asset.data.transforms.size(), prefab.transforms.size());
  for (usize i = 0; i < prefab.transforms.size(); ++i) {
    auto &expected = asset.data.transforms.at(i);
    auto &actual = prefab.transforms.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value.position, actual.value.position);
    EXPECT_EQ(expected.value.rotation, actual.value.rotation);
    EXPECT_EQ(expected.value.scale, actual.value.scale);
    EXPECT_EQ(expected.value.parent, actual.value.parent);
  }

  EXPECT_EQ(asset.data.names.size(), prefab.names.size());
  for (usize i = 0; i < prefab.names.size(); ++i) {
    auto &expected = asset.data.names.at(i);
    auto &actual = prefab.names.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value, actual.value);
  }

  EXPECT_EQ(asset.data.meshes.size(), prefab.meshes.size());
  for (usize i = 0; i < prefab.meshes.size(); ++i) {
    auto &expected = asset.data.meshes.at(i);
    auto &actual = prefab.meshes.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value, actual.value);
  }

  EXPECT_EQ(asset.data.meshRenderers.size(), prefab.meshRenderers.size());
  for (usize i = 0; i < prefab.meshRenderers.size(); ++i) {
    auto &expected = asset.data.meshRenderers.at(i);
    auto &actual = prefab.meshRenderers.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value.materials.size(), actual.value.materials.size());
    for (usize mi = 0; mi < expected.value.materials.size(); ++mi) {
      EXPECT_EQ(expected.value.materials.at(mi), actual.value.materials.at(mi));
    }
  }

  EXPECT_EQ(asset.data.skinnedMeshRenderers.size(),
            prefab.skinnedMeshRenderers.size());
  for (usize i = 0; i < prefab.skinnedMeshRenderers.size(); ++i) {
    auto &expected = asset.data.skinnedMeshRenderers.at(i);
    auto &actual = prefab.skinnedMeshRenderers.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value.materials.size(), actual.value.materials.size());
    for (usize mi = 0; mi < expected.value.materials.size(); ++mi) {
      EXPECT_EQ(expected.value.materials.at(mi), actual.value.materials.at(mi));
    }
  }

  EXPECT_EQ(asset.data.skeletons.size(), prefab.skeletons.size());
  for (usize i = 0; i < prefab.skeletons.size(); ++i) {
    auto &expected = asset.data.skeletons.at(i);
    auto &actual = prefab.skeletons.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value, actual.value);
  }

  EXPECT_EQ(asset.data.animations.size(), prefab.animations.size());
  for (usize i = 0; i < prefab.animations.size(); ++i) {
    auto &expected = asset.data.animations.at(i);
    auto &actual = prefab.animations.at(i);

    EXPECT_EQ(expected, actual);
  }

  EXPECT_EQ(asset.data.animators.size(), prefab.animators.size());
  for (usize i = 0; i < prefab.animators.size(); ++i) {
    auto &expected = asset.data.animators.at(i);
    auto &actual = prefab.animators.at(i);

    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value, actual.value);
  }

  EXPECT_EQ(asset.data.directionalLights.size(),
            prefab.directionalLights.size());
  for (usize i = 0; i < prefab.directionalLights.size(); ++i) {
    auto &expected = asset.data.directionalLights.at(i);
    auto &actual = prefab.directionalLights.at(i);

    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value.color, actual.value.color);
    EXPECT_EQ(expected.value.intensity, actual.value.intensity);
    EXPECT_EQ(expected.value.direction, glm::vec3(0.0f));
    EXPECT_EQ(actual.value.direction, glm::vec3(0.0f));
  }

  EXPECT_EQ(asset.data.pointLights.size(), prefab.pointLights.size());
  for (usize i = 0; i < prefab.pointLights.size(); ++i) {
    auto &expected = asset.data.pointLights.at(i);
    auto &actual = prefab.pointLights.at(i);

    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value.color, actual.value.color);
    EXPECT_EQ(expected.value.intensity, actual.value.intensity);
    EXPECT_EQ(expected.value.range, actual.value.range);
  }
}

TEST_F(AssetCachePrefabTest, LoadsPrefabWithMeshAnimationSkeleton) {
  // Create texture
  auto tempTextureHandle = cache.loadTexture(textureUuid).getData();
  auto tempTextureAsset = cache.getRegistry().getMeta(tempTextureHandle);
  cache.getRegistry().remove(tempTextureHandle);

  auto texturePath = cache.createTextureFromAsset(tempTextureAsset).getData();
  auto textureHandle = cache.loadTexture(textureUuid);

  // Create mesh
  quoll::AssetData<quoll::MeshAsset> meshData{};
  meshData.type = quoll::AssetType::SkinnedMesh;
  meshData.uuid = quoll::Uuid::generate();

  quoll::BaseGeometryAsset geometry{};
  geometry.positions.push_back({});
  geometry.normals.push_back({});
  geometry.tangents.push_back({});
  geometry.texCoords0.push_back({});
  geometry.texCoords1.push_back({});
  geometry.joints.push_back({});
  geometry.weights.push_back({});

  geometry.indices.push_back(0);
  meshData.data.geometries.push_back(geometry);
  auto meshPath = cache.createMeshFromAsset(meshData).getData();
  auto meshHandle = cache.loadMesh(meshData.uuid);

  // Create skeleton
  quoll::AssetData<quoll::SkeletonAsset> skeletonData{};
  skeletonData.uuid = quoll::Uuid::generate();

  auto skeletonPath = cache.createSkeletonFromAsset(skeletonData).getData();
  auto skeletonHandle = cache.loadSkeleton(skeletonData.uuid);

  // Create animation
  quoll::AssetData<quoll::AnimationAsset> animationData{};
  animationData.data.time = 2.5;
  animationData.uuid = quoll::Uuid::generate();
  auto animationPath = cache.createAnimationFromAsset(animationData).getData();
  auto animationHandle = cache.loadAnimation(animationData.uuid);

  // Create animator
  quoll::AssetData<quoll::AnimatorAsset> animatorData{};
  animatorData.data.states.push_back({"INITIAL"});
  animatorData.uuid = quoll::Uuid::generate();
  auto animatorPath = cache.createAnimatorFromAsset(animatorData).getData();
  auto animatorHandle = cache.loadAnimator(animatorData.uuid);

  // Create prefab
  quoll::AssetData<quoll::PrefabAsset> prefabData{};
  prefabData.uuid = quoll::Uuid::generate();
  prefabData.data.meshes.push_back({0U, meshHandle.getData()});
  prefabData.data.skeletons.push_back({0U, skeletonHandle.getData()});
  prefabData.data.animations.push_back(animationHandle.getData());
  prefabData.data.animators.push_back({0U, animatorHandle.getData()});

  auto prefabPath = cache.createPrefabFromAsset(prefabData);

  // Delete all existing assets
  cache.getRegistry().remove(textureHandle.getData());
  cache.getRegistry().remove(meshHandle.getData());
  cache.getRegistry().remove(skeletonHandle.getData());
  cache.getRegistry().remove(animationHandle.getData());
  cache.getRegistry().remove(animatorHandle.getData());

  auto prefabHandle = cache.loadPrefab(prefabData.uuid);
  EXPECT_NE(prefabHandle.getData(), quoll::AssetHandle<quoll::PrefabAsset>());

  auto &newPrefab = cache.getRegistry().get(prefabHandle.getData());

  // Validate mesh
  EXPECT_NE(newPrefab.meshes.at(0).value,
            quoll::AssetHandle<quoll::MeshAsset>());
  auto &newMesh = cache.getRegistry().getMeta(newPrefab.meshes.at(0).value);
  EXPECT_EQ(newMesh.uuid, meshData.uuid);

  // Validate skeleton
  EXPECT_NE(newPrefab.skeletons.at(0).value,
            quoll::AssetHandle<quoll::SkeletonAsset>());
  auto &newSkeleton =
      cache.getRegistry().getMeta(newPrefab.skeletons.at(0).value);
  EXPECT_EQ(newSkeleton.uuid, skeletonData.uuid);

  // Validate animation
  auto newAnimationHandle = newPrefab.animations.at(0);
  EXPECT_NE(newAnimationHandle, quoll::AssetHandle<quoll::AnimationAsset>());
  auto &newAnimation = cache.getRegistry().getMeta(newAnimationHandle);
  EXPECT_EQ(newAnimation.uuid, animationData.uuid);

  // Validate animator
  auto newAnimatorHandle = newPrefab.animators.at(0).value;
  EXPECT_NE(newAnimatorHandle, quoll::AssetHandle<quoll::AnimatorAsset>());
  auto &newAnimator = cache.getRegistry().getMeta(newAnimatorHandle);
  EXPECT_EQ(newAnimator.uuid, animatorData.uuid);
}
