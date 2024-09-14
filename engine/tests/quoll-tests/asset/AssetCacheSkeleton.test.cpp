#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/AssetFileHeader.h"
#include "quoll/asset/InputBinaryStream.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include <random>

class AssetCacheSkeletonTest : public AssetCacheTestBase {
public:
};

TEST_F(AssetCacheSkeletonTest, CreatesMetaFileFromAsset) {
  quoll::AssetData<quoll::SkeletonAsset> asset;
  asset.name = "test-skel0";
  asset.uuid = quoll::Uuid::generate();

  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<f32> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<u32> udist(0, 20);

    usize countJoints = 20;
    for (usize i = 0; i < countJoints; ++i) {
      asset.data.jointLocalPositions.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalRotations.push_back(
          glm::quat(dist(mt), dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalScales.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointInverseBindMatrices.push_back(glm::mat4{
          // row 0
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
          // row 1
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row 2
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row3
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
      });

      asset.data.jointParents.push_back(udist(mt));
      asset.data.jointNames.push_back("Joint " + std::to_string(i));
    }
  }

  auto filePath = cache.createFromData(asset);
  auto meta = cache.getAssetMeta(asset.uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Skeleton);
  EXPECT_EQ(meta.name, "test-skel0");
}

TEST_F(AssetCacheSkeletonTest, CreatesSkeletonFileFromSkeletonAsset) {
  quoll::AssetData<quoll::SkeletonAsset> asset;
  asset.name = "test-skel0";
  asset.uuid = quoll::Uuid::generate();

  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<f32> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<u32> udist(0, 20);

    usize countJoints = 20;
    for (usize i = 0; i < countJoints; ++i) {
      asset.data.jointLocalPositions.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalRotations.push_back(
          glm::quat(dist(mt), dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalScales.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointInverseBindMatrices.push_back(glm::mat4{
          // row 0
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
          // row 1
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row 2
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row3
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
      });

      asset.data.jointParents.push_back(udist(mt));
      asset.data.jointNames.push_back("Joint " + std::to_string(i));
    }
  }

  auto filePath = cache.createFromData(asset);

  quoll::InputBinaryStream file(filePath);
  EXPECT_TRUE(file.good());

  quoll::AssetFileHeader header;
  file.read(header);
  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.type, quoll::AssetType::Skeleton);

  u32 numJoints = 0;
  file.read(numJoints);
  EXPECT_EQ(numJoints, 20);

  std::vector<glm::vec3> actualPositions(numJoints);
  std::vector<glm::quat> actualRotations(numJoints);
  std::vector<glm::vec3> actualScales(numJoints);
  std::vector<glm::mat4> actualInverseBindMatrices(numJoints);
  std::vector<quoll::JointId> actualParents(numJoints);
  std::vector<quoll::String> actualNames(numJoints);
  file.read(actualPositions);
  file.read(actualRotations);
  file.read(actualScales);
  file.read(actualParents);
  file.read(actualInverseBindMatrices);
  file.read(actualNames);

  for (u32 i = 0; i < numJoints; ++i) {
    EXPECT_EQ(actualPositions.at(i), asset.data.jointLocalPositions.at(i));
    EXPECT_EQ(actualRotations.at(i), asset.data.jointLocalRotations.at(i));
    EXPECT_EQ(actualScales.at(i), asset.data.jointLocalScales.at(i));
    EXPECT_EQ(actualParents.at(i), asset.data.jointParents.at(i));
    EXPECT_EQ(actualInverseBindMatrices.at(i),
              asset.data.jointInverseBindMatrices.at(i));
    EXPECT_EQ(actualNames.at(i), asset.data.jointNames.at(i));
  }
}

TEST_F(AssetCacheSkeletonTest, LoadsSkeletonAssetFromFile) {
  quoll::AssetData<quoll::SkeletonAsset> asset;
  asset.name = "test-skel0";
  asset.uuid = quoll::Uuid::generate();

  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<f32> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<u32> udist(0, 20);

    usize countJoints = 20;
    for (usize i = 0; i < countJoints; ++i) {
      asset.data.jointLocalPositions.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalRotations.push_back(
          glm::quat(dist(mt), dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalScales.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointInverseBindMatrices.push_back(glm::mat4{
          // row 0
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
          // row 1
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row 2
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row3
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
      });

      asset.data.jointParents.push_back(udist(mt));
      asset.data.jointNames.push_back("Joint " + std::to_string(i));
    }
  }

  auto filePath = cache.createFromData(asset);
  auto res = requestAndWait<quoll::SkeletonAsset>(asset.uuid);
  ASSERT_TRUE(res);

  auto skeleton = res.data();
  EXPECT_NE(skeleton.handle(), quoll::AssetHandle<quoll::SkeletonAsset>());

  EXPECT_EQ(skeleton.meta().name, asset.name);

  for (usize i = 0; i < skeleton->jointLocalPositions.size(); ++i) {
    EXPECT_EQ(skeleton->jointLocalPositions.at(i),
              asset.data.jointLocalPositions.at(i));
    EXPECT_EQ(skeleton->jointLocalRotations.at(i),
              asset.data.jointLocalRotations.at(i));
    EXPECT_EQ(skeleton->jointLocalScales.at(i),
              asset.data.jointLocalScales.at(i));
    EXPECT_EQ(skeleton->jointParents.at(i), asset.data.jointParents.at(i));
    EXPECT_EQ(skeleton->jointInverseBindMatrices.at(i),
              asset.data.jointInverseBindMatrices.at(i));
    EXPECT_EQ(skeleton->jointNames.at(i), asset.data.jointNames.at(i));
  }
}
