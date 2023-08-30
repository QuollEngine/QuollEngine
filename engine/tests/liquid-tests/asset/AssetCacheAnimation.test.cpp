#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"
#include "liquid-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheAnimationTest : public AssetCacheTestBase {
public:
};

quoll::AssetData<quoll::AnimationAsset> createRandomizedAnimation() {
  quoll::AssetData<quoll::AnimationAsset> asset;
  asset.name = "test-anim0";
  asset.uuid = quoll::Uuid::generate();
  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<uint32_t> udist(0, 20);
    std::uniform_int_distribution<uint32_t> targetDist(0, 2);
    std::uniform_int_distribution<uint32_t> interpolationDist(0, 1);

    size_t countKeyframes = 5;
    size_t countKeyframeValues = 10;
    asset.data.time = static_cast<float>(countKeyframeValues) * 0.5f;

    for (size_t i = 0; i < countKeyframes; ++i) {
      quoll::KeyframeSequenceAsset keyframe;
      keyframe.interpolation =
          static_cast<quoll::KeyframeSequenceAssetInterpolation>(
              interpolationDist(mt));
      keyframe.target =
          static_cast<quoll::KeyframeSequenceAssetTarget>(targetDist(mt));
      keyframe.joint = udist(mt);
      keyframe.jointTarget = keyframe.joint == 10;
      keyframe.keyframeTimes.resize(countKeyframeValues);
      keyframe.keyframeValues.resize(countKeyframeValues);

      for (size_t j = 0; j < countKeyframeValues; ++j) {
        keyframe.keyframeTimes.at(j) = 0.5f * static_cast<float>(j);
        keyframe.keyframeValues.at(j) =
            glm::vec4(dist(mt), dist(mt), dist(mt), dist(mt));
      }

      asset.data.keyframes.push_back(keyframe);
    }
  }

  return asset;
}

TEST_F(AssetCacheAnimationTest, CreatesAnimationFile) {
  auto asset = createRandomizedAnimation();
  auto filePath = cache.createAnimationFromAsset(asset);
  quoll::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  quoll::AssetFileHeader header;
  file.read(header);
  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.type, quoll::AssetType::Animation);
  EXPECT_EQ(header.name, asset.name);

  float time = 0.0f;
  uint32_t numKeyframes = 0;

  file.read(time);
  file.read(numKeyframes);

  EXPECT_EQ(time, 5.0f);
  EXPECT_EQ(numKeyframes, 5);

  for (uint32_t i = 0; i < numKeyframes; ++i) {
    auto &keyframe = asset.data.keyframes.at(i);

    quoll::KeyframeSequenceAssetTarget target{0};
    quoll::KeyframeSequenceAssetInterpolation interpolation{0};
    bool jointTarget = false;
    quoll::JointId joint = 0;
    uint32_t numValues = 0;

    file.read(target);
    file.read(interpolation);
    file.read(jointTarget);
    file.read(joint);
    file.read(numValues);

    EXPECT_EQ(target, keyframe.target);
    EXPECT_EQ(interpolation, keyframe.interpolation);
    EXPECT_EQ(jointTarget, keyframe.jointTarget);
    EXPECT_EQ(joint, keyframe.joint);
    EXPECT_EQ(numValues, static_cast<uint32_t>(keyframe.keyframeValues.size()));

    std::vector<float> times(numValues);
    std::vector<glm::vec4> values(numValues);

    file.read(times);
    file.read(values);
    for (uint32_t i = 0; i < numValues; ++i) {
      EXPECT_EQ(times.at(i), keyframe.keyframeTimes.at(i));
      EXPECT_EQ(values.at(i), keyframe.keyframeValues.at(i));
    }
  }

  EXPECT_FALSE(std::filesystem::exists(
      filePath.getData().replace_extension("assetmeta")));
}

TEST_F(AssetCacheAnimationTest, LoadsAnimationAssetFromFile) {
  auto asset = createRandomizedAnimation();

  auto filePath = cache.createAnimationFromAsset(asset);
  auto handle = cache.loadAnimation(asset.uuid);
  EXPECT_FALSE(handle.hasError());
  EXPECT_NE(handle.getData(), quoll::AnimationAssetHandle::Null);

  auto &actual = cache.getRegistry().getAnimations().getAsset(handle.getData());
  EXPECT_EQ(actual.name, asset.name);
  EXPECT_EQ(actual.type, quoll::AssetType::Animation);

  EXPECT_EQ(actual.data.time, asset.data.time);
  EXPECT_EQ(actual.data.keyframes.size(), asset.data.keyframes.size());
  for (size_t i = 0; i < asset.data.keyframes.size(); ++i) {
    auto &expectedKf = asset.data.keyframes.at(i);
    auto &actualKf = actual.data.keyframes.at(i);

    EXPECT_EQ(expectedKf.target, actualKf.target);
    EXPECT_EQ(expectedKf.interpolation, actualKf.interpolation);
    EXPECT_EQ(expectedKf.jointTarget, actualKf.jointTarget);
    EXPECT_EQ(expectedKf.joint, actualKf.joint);
    EXPECT_EQ(expectedKf.keyframeTimes.size(), actualKf.keyframeTimes.size());
    EXPECT_EQ(expectedKf.keyframeValues.size(), actualKf.keyframeValues.size());

    for (size_t j = 0; j < expectedKf.keyframeTimes.size(); ++j) {
      EXPECT_EQ(expectedKf.keyframeTimes.at(j), actualKf.keyframeTimes.at(j));
      EXPECT_EQ(expectedKf.keyframeValues.at(j), actualKf.keyframeValues.at(j));
    }
  }
}
