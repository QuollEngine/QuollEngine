#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"

class AssetCacheTest : public ::testing::Test {
public:
  AssetCacheTest() : cache(FixturesPath) {}

  liquid::AssetData<liquid::PrefabAsset> createPrefabAsset() {
    liquid::AssetData<liquid::PrefabAsset> asset;
    asset.name = "test-prefab0";

    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<int32_t> idist(-1, 2);
    std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
    std::uniform_real_distribution<float> distPositive(0.0f, 10.0f);

    uint32_t numTransforms = 5;
    uint32_t numMeshes = 5;
    uint32_t numAnimators = 4;
    uint32_t numAnimations = 3;
    uint32_t numSkeletons = 3;
    uint32_t numDirectionalLights = 2;
    uint32_t numPointLights = 4;
    uint32_t numNames = 3;

    for (uint32_t i = 0; i < numTransforms; ++i) {
      liquid::PrefabTransformData transform{};
      transform.position = {dist(mt), dist(mt), dist(mt)};
      transform.rotation = {dist(mt), dist(mt), dist(mt), dist(mt)};
      transform.scale = {dist(mt), dist(mt), dist(mt)};
      transform.parent = idist(mt);

      asset.data.transforms.push_back({i, transform});
    }

    for (uint32_t i = 0; i < numNames; ++i) {
      liquid::String name = "Test name " + std::to_string(i);

      asset.data.names.push_back({i, name});
    }

    for (uint32_t i = 0; i < numDirectionalLights; ++i) {
      liquid::DirectionalLight light{};
      light.color = {distColor(mt), distColor(mt), distColor(mt),
                     distColor(mt)};
      light.intensity = distPositive(mt);

      asset.data.directionalLights.push_back({i, light});
    }

    for (uint32_t i = 0; i < numPointLights; ++i) {
      liquid::PointLight light{};
      light.color = {distColor(mt), distColor(mt), distColor(mt),
                     distColor(mt)};
      light.intensity = distPositive(mt);
      light.range = distPositive(mt);

      asset.data.pointLights.push_back({i, light});
    }

    for (uint32_t i = 0; i < numMeshes; ++i) {
      liquid::AssetData<liquid::MeshAsset> mesh;
      mesh.path =
          FixturesPath / ("meshes/mesh-" + std::to_string(i) + ".lqmesh");
      auto handle = cache.getRegistry().getMeshes().addAsset(mesh);
      asset.data.meshes.push_back({i, handle});
    }

    // Add two more entities that point to the same
    // meshes to test that existing meshes are always
    // referenced instead of added again
    for (uint32_t i = 0; i < 2; ++i) {
      auto handle = asset.data.meshes.at(static_cast<size_t>(i)).value;
      asset.data.meshes.push_back({numMeshes + i, handle});
    }

    for (uint32_t i = 0; i < numSkeletons; ++i) {
      liquid::AssetData<liquid::SkinnedMeshAsset> mesh;
      mesh.path =
          FixturesPath / ("meshes/smesh-" + std::to_string(i) + ".lqmesh");
      auto handle = cache.getRegistry().getSkinnedMeshes().addAsset(mesh);
      asset.data.skinnedMeshes.push_back({i, handle});
    }

    // Add two more entities that point to the same
    // meshes to test that existing meshes are always
    // referenced instead of added again
    for (uint32_t i = 0; i < 2; ++i) {
      auto handle = asset.data.skinnedMeshes.at(static_cast<size_t>(i)).value;
      asset.data.skinnedMeshes.push_back({numSkeletons + i, handle});
    }

    for (uint32_t i = 0; i < numSkeletons; ++i) {
      liquid::AssetData<liquid::SkeletonAsset> skeleton;
      skeleton.path =
          FixturesPath / ("skeletons/skel-" + std::to_string(i) + ".lqskel");
      auto handle = cache.getRegistry().getSkeletons().addAsset(skeleton);
      asset.data.skeletons.push_back({i, handle});
    }

    // Add two more entities that point to the same
    // skeletons to test that existing skeletons are always
    // referenced instead of added again
    for (uint32_t i = 0; i < 2; ++i) {
      auto handle = asset.data.skeletons.at(static_cast<size_t>(i)).value;
      asset.data.skeletons.push_back({numSkeletons + i, handle});
    }

    for (uint32_t i = 0; i < numAnimations; ++i) {
      liquid::AssetData<liquid::AnimationAsset> animation;
      animation.path =
          FixturesPath / ("skeletons/anim-" + std::to_string(i) + ".lqanim");
      auto handle = cache.getRegistry().getAnimations().addAsset(animation);
      asset.data.animations.push_back(handle);
    }

    for (uint32_t i = 0; i < numAnimators; ++i) {
      liquid::AssetData<liquid::AnimatorAsset> animator;
      animator.path = FixturesPath /
                      ("skeletons/animator-" + std::to_string(i) + ".animator");
      auto handle = cache.getRegistry().getAnimators().addAsset(animator);
      asset.data.animators.push_back({i, handle});
    }

    // Add two more entities that point to same animations
    // to test that existing animations are always
    // referenced instead of added again
    for (uint32_t i = 0; i < 2; ++i) {
      auto handle = asset.data.animators.at(i).value;
      asset.data.animators.push_back({numAnimators + i, handle});
    }

    return asset;
  }

  liquid::AssetCache cache;
};

TEST_F(AssetCacheTest, CreatesPrefabFile) {
  auto asset = createPrefabAsset();

  auto filePath = cache.createPrefabFromAsset(asset);
  liquid::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::AssetFileMagicLength, '$');
  file.read(magic.data(), magic.length());
  file.read(header.version);
  file.read(header.type);
  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(header.type, liquid::AssetType::Prefab);

  std::vector<liquid::String> actualMeshes;
  {
    auto &expected = asset.data.meshes;
    auto &map = cache.getRegistry().getMeshes();
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, static_cast<uint32_t>(expected.size() - 2));
    actualMeshes.resize(numAssets);
    file.read(actualMeshes);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto expectedString =
          std::filesystem::relative(map.getAsset(expected.at(i).value).path,
                                    FixturesPath)
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actualMeshes.at(i));
    }
  }

  std::vector<liquid::String> actualSkinnedMeshes;
  {
    auto &expected = asset.data.skinnedMeshes;
    auto &map = cache.getRegistry().getSkinnedMeshes();
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, 3);
    actualSkinnedMeshes.resize(numAssets);
    file.read(actualSkinnedMeshes);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto expectedString =
          std::filesystem::relative(map.getAsset(expected.at(i).value).path,
                                    FixturesPath)
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actualSkinnedMeshes.at(i));
    }
  }

  std::vector<liquid::String> actualSkeletons;
  {
    auto &expected = asset.data.skeletons;
    auto &map = cache.getRegistry().getSkeletons();
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, 3);
    actualSkeletons.resize(numAssets);
    file.read(actualSkeletons);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto expectedString =
          std::filesystem::relative(map.getAsset(expected.at(i).value).path,
                                    FixturesPath)
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actualSkeletons.at(i));
    }
  }

  std::vector<liquid::String> actualAnimations;
  {
    auto &expected = asset.data.animations;
    auto &map = cache.getRegistry().getAnimations();
    auto &actual = actualAnimations;
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, 3);
    actualAnimations.resize(numAssets);
    file.read(actual);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto animation = expected.at(i);

      auto expectedString =
          std::filesystem::relative(map.getAsset(animation).path, FixturesPath)
              .string();

      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  std::vector<liquid::String> actualAnimators;
  {
    auto &expected = asset.data.animators;
    auto &map = cache.getRegistry().getAnimators();
    auto &actual = actualAnimators;
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, 4);
    actualAnimators.resize(numAssets);
    file.read(actual);

    for (uint32_t i = 0; i < numAssets; ++i) {
      uint32_t entityId = (i / 3);
      auto animation = expected.at(i).value;

      auto expectedString =
          std::filesystem::relative(map.getAsset(animation).path, FixturesPath)
              .string();

      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 5);
    std::vector<liquid::PrefabTransformData> transforms(numComponents);
    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 0;
      file.read(entity);

      glm::vec3 position;
      glm::quat rotation;
      glm::vec3 scale;
      int32_t parent = -1;
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
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 3);

    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      liquid::String name;
      file.read(name);

      EXPECT_EQ(asset.data.names.at(i).value, name);
    }
  }

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 7);
    auto &map = cache.getRegistry().getMeshes();
    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      uint32_t meshIndex = 999;
      file.read(meshIndex);
      EXPECT_EQ(meshIndex, i % 5);

      auto &expected = map.getAsset(asset.data.meshes.at(i).value);

      auto expectedString =
          std::filesystem::relative(expected.path, FixturesPath).string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actualMeshes.at(meshIndex));
    }
  }

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 5);
    auto &map = cache.getRegistry().getSkinnedMeshes();
    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      uint32_t meshIndex = 999;
      file.read(meshIndex);
      EXPECT_EQ(meshIndex, i % 3);

      auto &expected = map.getAsset(asset.data.skinnedMeshes.at(i).value);

      auto expectedString =
          std::filesystem::relative(expected.path, FixturesPath).string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actualSkinnedMeshes.at(meshIndex));
    }
  }

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 5);
    auto &map = cache.getRegistry().getSkeletons();
    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      uint32_t skeletonIndex = 999;
      file.read(skeletonIndex);
      EXPECT_EQ(skeletonIndex, i % 3);

      auto &expected = map.getAsset(asset.data.skeletons.at(i).value);

      auto expectedString =
          std::filesystem::relative(expected.path, FixturesPath).string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actualSkeletons.at(skeletonIndex));
    }
  }

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 3);
    auto &map = cache.getRegistry().getAnimations();

    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t animatorIndex = 999;
      file.read(animatorIndex);

      auto &expected = map.getAsset(asset.data.animations.at(i));

      auto expectedString =
          std::filesystem::relative(expected.path, FixturesPath).string();

      EXPECT_EQ(expectedString, actualAnimations.at(animatorIndex));
    }
  }

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 6);
    auto &map = cache.getRegistry().getAnimators();

    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      uint32_t animatorIndex = 999;
      file.read(animatorIndex);

      auto &expected = map.getAsset(asset.data.animators.at(i).value);

      auto expectedString =
          std::filesystem::relative(expected.path, FixturesPath).string();

      EXPECT_EQ(expectedString, actualAnimators.at(animatorIndex));
    }
  }

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 2);

    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      glm::vec4 color{999.0f};
      float intensity = 0.0f;
      file.read(color);
      file.read(intensity);

      EXPECT_EQ(asset.data.directionalLights.at(i).value.color, color);
      EXPECT_EQ(asset.data.directionalLights.at(i).value.intensity, intensity);
    }
  }

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 4);

    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      glm::vec4 color{999.0f};
      float intensity = 0.0f;
      float range = 99.0f;
      file.read(color);
      file.read(intensity);
      file.read(range);

      EXPECT_EQ(asset.data.pointLights.at(i).value.color, color);
      EXPECT_EQ(asset.data.pointLights.at(i).value.intensity, intensity);
      EXPECT_EQ(asset.data.pointLights.at(i).value.range, range);
    }
  }
}

TEST_F(AssetCacheTest, FailsLoadingPrefabIfPrefabHasNoComponents) {
  liquid::AssetData<liquid::PrefabAsset> asset;
  asset.name = "test-prefab0";
  auto filePath = cache.createPrefabFromAsset(asset);

  auto res = cache.loadPrefabFromFile(filePath.getData());
  EXPECT_TRUE(res.hasError());
  EXPECT_FALSE(res.hasData());
  EXPECT_FALSE(res.hasWarnings());
}

TEST_F(AssetCacheTest, LoadsPrefabFile) {
  auto asset = createPrefabAsset();
  auto filePath = cache.createPrefabFromAsset(asset);
  auto handle = cache.loadPrefabFromFile(filePath.getData());
  EXPECT_NE(handle.getData(), liquid::PrefabAssetHandle::Null);

  auto &prefab = cache.getRegistry().getPrefabs().getAsset(handle.getData());

  EXPECT_EQ(asset.data.transforms.size(), prefab.data.transforms.size());
  for (size_t i = 0; i < prefab.data.transforms.size(); ++i) {
    auto &expected = asset.data.transforms.at(i);
    auto &actual = prefab.data.transforms.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value.position, actual.value.position);
    EXPECT_EQ(expected.value.rotation, actual.value.rotation);
    EXPECT_EQ(expected.value.scale, actual.value.scale);
    EXPECT_EQ(expected.value.parent, actual.value.parent);
  }

  EXPECT_EQ(asset.data.names.size(), prefab.data.names.size());
  for (size_t i = 0; i < prefab.data.names.size(); ++i) {
    auto &expected = asset.data.names.at(i);
    auto &actual = prefab.data.names.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value, actual.value);
  }

  EXPECT_EQ(asset.data.meshes.size(), prefab.data.meshes.size());
  for (size_t i = 0; i < prefab.data.meshes.size(); ++i) {
    auto &expected = asset.data.meshes.at(i);
    auto &actual = prefab.data.meshes.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value, actual.value);
  }

  EXPECT_EQ(asset.data.skinnedMeshes.size(), prefab.data.skinnedMeshes.size());
  for (size_t i = 0; i < prefab.data.skinnedMeshes.size(); ++i) {
    auto &expected = asset.data.skinnedMeshes.at(i);
    auto &actual = prefab.data.skinnedMeshes.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value, actual.value);
  }

  EXPECT_EQ(asset.data.skeletons.size(), prefab.data.skeletons.size());
  for (size_t i = 0; i < prefab.data.skeletons.size(); ++i) {
    auto &expected = asset.data.skeletons.at(i);
    auto &actual = prefab.data.skeletons.at(i);
    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value, actual.value);
  }

  EXPECT_EQ(asset.data.animations.size(), prefab.data.animations.size());
  for (size_t i = 0; i < prefab.data.animations.size(); ++i) {
    auto &expected = asset.data.animations.at(i);
    auto &actual = prefab.data.animations.at(i);

    EXPECT_EQ(expected, actual);
  }

  EXPECT_EQ(asset.data.animators.size(), prefab.data.animators.size());
  for (size_t i = 0; i < prefab.data.animators.size(); ++i) {
    auto &expected = asset.data.animators.at(i);
    auto &actual = prefab.data.animators.at(i);

    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value, actual.value);
  }

  EXPECT_EQ(asset.data.directionalLights.size(),
            prefab.data.directionalLights.size());
  for (size_t i = 0; i < prefab.data.directionalLights.size(); ++i) {
    auto &expected = asset.data.directionalLights.at(i);
    auto &actual = prefab.data.directionalLights.at(i);

    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value.color, actual.value.color);
    EXPECT_EQ(expected.value.intensity, actual.value.intensity);
    EXPECT_EQ(expected.value.direction, glm::vec3(0.0f));
    EXPECT_EQ(actual.value.direction, glm::vec3(0.0f));
  }

  EXPECT_EQ(asset.data.pointLights.size(), prefab.data.pointLights.size());
  for (size_t i = 0; i < prefab.data.pointLights.size(); ++i) {
    auto &expected = asset.data.pointLights.at(i);
    auto &actual = prefab.data.pointLights.at(i);

    EXPECT_EQ(expected.entity, actual.entity);
    EXPECT_EQ(expected.value.color, actual.value.color);
    EXPECT_EQ(expected.value.intensity, actual.value.intensity);
    EXPECT_EQ(expected.value.range, actual.value.range);
  }
}

TEST_F(AssetCacheTest, LoadsPrefabWithMeshAnimationSkeleton) {
  // Create texture
  auto textureHandle = cache.loadTextureFromFile(FixturesPath / "1x1-2d.ktx");

  // Create material
  liquid::AssetData<liquid::MaterialAsset> materialData{};
  materialData.name = "test-prefab-mesh-material";
  materialData.data.baseColorTexture = textureHandle.getData();
  auto materialPath = cache.createMaterialFromAsset(materialData);
  auto materialHandle = cache.loadMaterialFromFile(materialPath.getData());

  // Create mesh
  liquid::AssetData<liquid::SkinnedMeshAsset> meshData{};
  meshData.name = "test-prefab-mesh";
  liquid::BaseGeometryAsset<liquid::SkinnedVertex> geometry;
  geometry.material = materialHandle.getData();
  geometry.vertices.push_back({1.0f});
  geometry.indices.push_back(0);
  meshData.data.geometries.push_back(geometry);
  auto meshPath = cache.createSkinnedMeshFromAsset(meshData);
  auto meshHandle = cache.loadSkinnedMeshFromFile(meshPath.getData());

  // Create skeleton
  liquid::AssetData<liquid::SkeletonAsset> skeletonData{};
  skeletonData.name = "test-prefab-skeleton";
  auto skeletonPath = cache.createSkeletonFromAsset(skeletonData);
  auto skeletonHandle = cache.loadSkeletonFromFile(skeletonPath.getData());

  // Create animation
  liquid::AssetData<liquid::AnimationAsset> animationData{};
  animationData.name = "test-prefab-animation";
  animationData.data.time = 2.5;
  auto animationPath = cache.createAnimationFromAsset(animationData).getData();
  auto animationHandle = cache.loadAnimationFromFile(animationPath);

  // Create animator
  liquid::AssetData<liquid::AnimatorAsset> animatorData{};
  animatorData.relativePath = "test-prefab-animator.animator";
  animatorData.data.states.push_back({"INITIAL"});
  auto animatorPath = cache.createAnimatorFromAsset(animatorData).getData();
  auto animatorHandle = cache.loadAnimatorFromFile(animatorPath);

  // Create prefab
  liquid::AssetData<liquid::PrefabAsset> prefabData{};
  prefabData.data.skinnedMeshes.push_back({0U, meshHandle.getData()});
  prefabData.data.skeletons.push_back({0U, skeletonHandle.getData()});
  prefabData.data.animations.push_back(animationHandle.getData());
  prefabData.data.animators.push_back({0U, animatorHandle.getData()});

  auto prefabPath = cache.createPrefabFromAsset(prefabData);

  // Delete all existing assets
  cache.getRegistry().getTextures().deleteAsset(textureHandle.getData());
  cache.getRegistry().getMaterials().deleteAsset(materialHandle.getData());
  cache.getRegistry().getSkinnedMeshes().deleteAsset(meshHandle.getData());
  cache.getRegistry().getSkeletons().deleteAsset(skeletonHandle.getData());
  cache.getRegistry().getAnimations().deleteAsset(animationHandle.getData());
  cache.getRegistry().getAnimators().deleteAsset(animatorHandle.getData());

  auto prefabHandle = cache.loadPrefabFromFile(prefabPath.getData());
  EXPECT_NE(prefabHandle.getData(), liquid::PrefabAssetHandle::Null);

  auto &newPrefab =
      cache.getRegistry().getPrefabs().getAsset(prefabHandle.getData());

  // Validate mesh
  EXPECT_NE(newPrefab.data.skinnedMeshes.at(0).value,
            liquid::SkinnedMeshAssetHandle::Null);
  auto &newMesh = cache.getRegistry().getSkinnedMeshes().getAsset(
      newPrefab.data.skinnedMeshes.at(0).value);
  EXPECT_NE(newMesh.data.geometries.at(0).material,
            liquid::MaterialAssetHandle::Null);

  // Validate material
  auto &newMaterial = cache.getRegistry().getMaterials().getAsset(
      newMesh.data.geometries.at(0).material);
  EXPECT_NE(newMaterial.data.baseColorTexture,
            liquid::TextureAssetHandle::Null);

  // Validate texture
  auto &newTexture = cache.getRegistry().getTextures().getAsset(
      newMaterial.data.baseColorTexture);
  EXPECT_EQ(newTexture.name, "1x1-2d.ktx");

  // Validate skeleton
  EXPECT_NE(newPrefab.data.skeletons.at(0).value,
            liquid::SkeletonAssetHandle::Null);
  auto &newSkeleton = cache.getRegistry().getSkeletons().getAsset(
      newPrefab.data.skeletons.at(0).value);
  EXPECT_EQ(newSkeleton.name, "test-prefab-skeleton.lqskel");

  // Validate animation
  auto newAnimationHandle = newPrefab.data.animations.at(0);
  EXPECT_NE(newAnimationHandle, liquid::AnimationAssetHandle::Null);
  auto &newAnimation =
      cache.getRegistry().getAnimations().getAsset(newAnimationHandle);
  EXPECT_EQ(newAnimation.name, "test-prefab-animation.lqanim");

  // Validate animator
  auto newAnimatorHandle = newPrefab.data.animators.at(0).value;
  EXPECT_NE(newAnimatorHandle, liquid::AnimatorAssetHandle::Null);
  auto &newAnimator =
      cache.getRegistry().getAnimators().getAsset(newAnimatorHandle);
  EXPECT_EQ(newAnimator.relativePath, "test-prefab-animator.animator");
}
