#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include <gtest/gtest.h>
#include <glm/gtx/string_cast.hpp>

class AssetManagerTest : public ::testing::Test {
public:
  AssetManagerTest() : manager(std::filesystem::current_path()) {}

  liquid::AssetData<liquid::PrefabAsset> createPrefabAsset() {
    liquid::AssetData<liquid::PrefabAsset> asset;
    asset.name = "test-prefab0";

    uint32_t numMeshes = 5;
    uint32_t numAnimators = 4;
    uint32_t numAnimationsPerAnimator = 3;
    uint32_t numSkeletons = 3;
    for (uint32_t i = 0; i < numMeshes; ++i) {
      liquid::AssetData<liquid::MeshAsset> mesh;
      mesh.path = std::filesystem::current_path() /
                  ("meshes/mesh-" + std::to_string(i) + ".lqmesh");
      auto handle = manager.getRegistry().getMeshes().addAsset(mesh);
      asset.data.meshes.push_back({i, handle});
    }

    for (uint32_t i = 0; i < numSkeletons; ++i) {
      liquid::AssetData<liquid::SkinnedMeshAsset> mesh;
      mesh.path = std::filesystem::current_path() /
                  ("meshes/smesh-" + std::to_string(i) + ".lqmesh");
      auto handle = manager.getRegistry().getSkinnedMeshes().addAsset(mesh);
      asset.data.skinnedMeshes.push_back({i, handle});
    }

    for (uint32_t i = 0; i < numSkeletons; ++i) {
      liquid::AssetData<liquid::SkeletonAsset> skeleton;
      skeleton.path = std::filesystem::current_path() /
                      ("skeletons/skel-" + std::to_string(i) + ".lqskel");
      auto handle = manager.getRegistry().getSkeletons().addAsset(skeleton);
      asset.data.skeletons.push_back({i, handle});
    }

    asset.data.animators.resize(numAnimators);
    for (uint32_t i = 0; i < numAnimators * numAnimationsPerAnimator; ++i) {
      liquid::AssetData<liquid::AnimationAsset> animation;
      animation.path = std::filesystem::current_path() /
                       ("skeletons/anim-" + std::to_string(i) + ".lqanim");
      auto handle = manager.getRegistry().getAnimations().addAsset(animation);

      uint32_t entityId = (i / numAnimationsPerAnimator);

      asset.data.animators.at(entityId).entity = entityId;
      asset.data.animators.at(entityId).value.animations.push_back(handle);
    }

    return asset;
  }

  liquid::AssetManager manager;
};

TEST_F(AssetManagerTest, CreatesPrefabFile) {
  auto asset = createPrefabAsset();

  auto filePath = manager.createPrefabFromAsset(asset);
  liquid::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), magic.length());
  file.read(header.version);
  file.read(header.type);
  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(header.type, liquid::AssetType::Prefab);

  {
    auto &expected = asset.data.meshes;
    auto &map = manager.getRegistry().getMeshes();
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, static_cast<uint32_t>(expected.size()));
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto expectedString =
          std::filesystem::relative(map.getAsset(expected.at(i).value).path,
                                    std::filesystem::current_path())
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  {
    auto &expected = asset.data.skinnedMeshes;
    auto &map = manager.getRegistry().getSkinnedMeshes();
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, static_cast<uint32_t>(expected.size()));
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto expectedString =
          std::filesystem::relative(map.getAsset(expected.at(i).value).path,
                                    std::filesystem::current_path())
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  {
    auto &expected = asset.data.skeletons;
    auto &map = manager.getRegistry().getSkeletons();
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, static_cast<uint32_t>(expected.size()));
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto expectedString =
          std::filesystem::relative(map.getAsset(expected.at(i).value).path,
                                    std::filesystem::current_path())
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  std::vector<liquid::String> actualAnimations;
  {
    auto &expected = asset.data.animators;
    auto &map = manager.getRegistry().getAnimations();
    auto &actual = actualAnimations;
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, 12);
    actualAnimations.resize(numAssets);
    file.read(actual);

    for (uint32_t i = 0; i < numAssets; ++i) {
      uint32_t entityId = (i / 3);
      auto animation = expected.at(entityId).value.animations.at(i % 3);

      auto expectedString =
          std::filesystem::relative(map.getAsset(animation).path,
                                    std::filesystem::current_path())
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  uint32_t dummy = 0;
  file.read(dummy);
  EXPECT_EQ(dummy, 0);
  file.read(dummy);
  EXPECT_EQ(dummy, 0);
  file.read(dummy);
  EXPECT_EQ(dummy, 0);
  file.read(dummy);

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 4);
    auto &map = manager.getRegistry().getAnimations();

    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      uint32_t numAnimations = 0;
      file.read(numAnimations);
      EXPECT_EQ(numAnimations, 3);

      std::vector<uint32_t> animations(numAnimations);
      file.read(animations);

      EXPECT_EQ(asset.data.animators.at(i).value.animations.size(),
                animations.size());

      for (size_t j = 0; j < animations.size(); ++j) {
        auto &expected =
            map.getAsset(asset.data.animators.at(i).value.animations.at(j));

        auto expectedString =
            std::filesystem::relative(expected.path,
                                      std::filesystem::current_path())
                .string();
        std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

        EXPECT_EQ(expectedString, actualAnimations.at(animations.at(j)));
      }
    }
  }
}

TEST_F(AssetManagerTest, LoadsPrefabFile) {
  auto asset = createPrefabAsset();
  auto filePath = manager.createPrefabFromAsset(asset);
  auto handle = manager.loadPrefabFromFile(filePath.getData());
  EXPECT_NE(handle.getData(), liquid::PrefabAssetHandle::Invalid);

  auto &prefab = manager.getRegistry().getPrefabs().getAsset(handle.getData());

  EXPECT_EQ(asset.data.meshes.size(), prefab.data.meshes.size());
  for (size_t i = 0; i < prefab.data.meshes.size(); ++i) {
    auto expected = asset.data.meshes.at(i).value;
    auto actual = prefab.data.meshes.at(i).value;
    EXPECT_EQ(expected, actual);
  }

  EXPECT_EQ(asset.data.skinnedMeshes.size(), prefab.data.skinnedMeshes.size());
  for (size_t i = 0; i < prefab.data.skinnedMeshes.size(); ++i) {
    auto expected = asset.data.skinnedMeshes.at(i).value;
    auto actual = prefab.data.skinnedMeshes.at(i).value;
    EXPECT_EQ(expected, actual);
  }

  EXPECT_EQ(asset.data.skeletons.size(), prefab.data.skeletons.size());
  for (size_t i = 0; i < prefab.data.skeletons.size(); ++i) {
    auto expected = asset.data.skeletons.at(i).value;
    auto actual = prefab.data.skeletons.at(i).value;
    EXPECT_EQ(expected, actual);
  }

  EXPECT_EQ(asset.data.animators.size(), prefab.data.animators.size());
  for (size_t i = 0; i < prefab.data.animators.size(); ++i) {
    auto expected = asset.data.animators.at(i).value;
    auto actual = prefab.data.animators.at(i).value;

    EXPECT_EQ(expected.animations.size(), actual.animations.size());

    for (size_t j = 0; j < expected.animations.size(); ++j) {
      EXPECT_EQ(expected.animations.at(j), actual.animations.at(j));
    }
  }
}

TEST_F(AssetManagerTest, LoadsPrefabWithMeshAnimationSkeleton) {

  // Create texture
  auto textureHandle = manager.loadTextureFromFile("1x1-2d.ktx");

  // Create material
  liquid::AssetData<liquid::MaterialAsset> materialData{};
  materialData.name = "test-prefab-mesh-material";
  materialData.data.baseColorTexture = textureHandle.getData();
  auto materialPath = manager.createMaterialFromAsset(materialData);
  auto materialHandle = manager.loadMaterialFromFile(materialPath.getData());

  // Create mesh
  liquid::AssetData<liquid::SkinnedMeshAsset> meshData{};
  meshData.name = "test-prefab-mesh";
  liquid::BaseGeometryAsset<liquid::SkinnedVertex> geometry;
  geometry.material = materialHandle.getData();
  meshData.data.geometries.push_back(geometry);
  auto meshPath = manager.createSkinnedMeshFromAsset(meshData);
  auto meshHandle = manager.loadSkinnedMeshFromFile(meshPath.getData());

  // Create skeleton
  liquid::AssetData<liquid::SkeletonAsset> skeletonData{};
  skeletonData.name = "test-prefab-skeleton";
  auto skeletonPath = manager.createSkeletonFromAsset(skeletonData);
  auto skeletonHandle = manager.loadSkeletonFromFile(skeletonPath.getData());

  // Create animation
  liquid::AssetData<liquid::AnimationAsset> animationData{};
  animationData.name = "test-prefab-animation";
  animationData.data.time = 2.5;
  auto animationPath =
      manager.createAnimationFromAsset(animationData).getData();
  auto animationHandle = manager.loadAnimationFromFile(animationPath);

  // Create prefab
  liquid::AssetData<liquid::PrefabAsset> prefabData{};
  prefabData.data.skinnedMeshes.push_back({0U, meshHandle.getData()});
  prefabData.data.skeletons.push_back({0U, skeletonHandle.getData()});
  liquid::PrefabComponent<liquid::AnimatorComponent> animator{};
  animator.entity = 0;
  animator.value.animations.push_back(animationHandle.getData());
  prefabData.data.animators.push_back(animator);

  auto prefabPath = manager.createPrefabFromAsset(prefabData);

  // Delete all existing assets
  manager.getRegistry().getTextures().deleteAsset(textureHandle.getData());
  manager.getRegistry().getMaterials().deleteAsset(materialHandle.getData());
  manager.getRegistry().getSkinnedMeshes().deleteAsset(meshHandle.getData());
  manager.getRegistry().getSkeletons().deleteAsset(skeletonHandle.getData());
  manager.getRegistry().getAnimations().deleteAsset(animationHandle.getData());

  auto prefabHandle = manager.loadPrefabFromFile(prefabPath.getData());
  EXPECT_NE(prefabHandle.getData(), liquid::PrefabAssetHandle::Invalid);

  auto &newPrefab =
      manager.getRegistry().getPrefabs().getAsset(prefabHandle.getData());

  // Validate mesh
  EXPECT_NE(newPrefab.data.skinnedMeshes.at(0).value,
            liquid::SkinnedMeshAssetHandle::Invalid);
  auto &newMesh = manager.getRegistry().getSkinnedMeshes().getAsset(
      newPrefab.data.skinnedMeshes.at(0).value);
  EXPECT_NE(newMesh.data.geometries.at(0).material,
            liquid::MaterialAssetHandle::Invalid);

  // Validate material
  auto &newMaterial = manager.getRegistry().getMaterials().getAsset(
      newMesh.data.geometries.at(0).material);
  EXPECT_NE(newMaterial.data.baseColorTexture,
            liquid::TextureAssetHandle::Invalid);

  // Validate texture
  auto &newTexture = manager.getRegistry().getTextures().getAsset(
      newMaterial.data.baseColorTexture);
  EXPECT_EQ(newTexture.name, "1x1-2d.ktx");

  // Validate skeleton
  EXPECT_NE(newPrefab.data.skeletons.at(0).value,
            liquid::SkeletonAssetHandle::Invalid);
  auto &newSkeleton = manager.getRegistry().getSkeletons().getAsset(
      newPrefab.data.skeletons.at(0).value);
  EXPECT_EQ(newSkeleton.name, "test-prefab-skeleton.lqskel");

  // Validate animation
  auto newAnimationHandle =
      newPrefab.data.animators.at(0).value.animations.at(0);
  EXPECT_NE(newAnimationHandle, liquid::AnimationAssetHandle::Invalid);
  auto &newAnimation =
      manager.getRegistry().getAnimations().getAsset(newAnimationHandle);
  EXPECT_EQ(newAnimation.name, "test-prefab-animation.lqanim");
}