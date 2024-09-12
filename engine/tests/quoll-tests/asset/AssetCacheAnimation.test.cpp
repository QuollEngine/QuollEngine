#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/AssetFileHeader.h"
#include "quoll/asset/InputBinaryStream.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include <random>

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
    std::uniform_real_distribution<f32> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<u32> udist(0, 20);
    std::uniform_int_distribution<u32> targetDist(0, 2);
    std::uniform_int_distribution<u32> interpolationDist(0, 1);

    usize countKeyframes = 5;
    usize countKeyframeValues = 10;
    asset.data.time = static_cast<f32>(countKeyframeValues) * 0.5f;

    for (usize i = 0; i < countKeyframes; ++i) {
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

      for (usize j = 0; j < countKeyframeValues; ++j) {
        keyframe.keyframeTimes.at(j) = 0.5f * static_cast<f32>(j);
        keyframe.keyframeValues.at(j) =
            glm::vec4(dist(mt), dist(mt), dist(mt), dist(mt));
      }

      asset.data.keyframes.push_back(keyframe);
    }
  }

  return asset;
}

TEST_F(AssetCacheAnimationTest, CreatesMetaFileFromAsset) {
  auto asset = createRandomizedAnimation();
  auto filePath = cache.createFromData(asset);
  auto meta = cache.getAssetMeta(asset.uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Animation);
  EXPECT_EQ(meta.name, "test-anim0");
}

TEST_F(AssetCacheAnimationTest, CreatesAnimationFile) {
  auto asset = createRandomizedAnimation();
  auto filePath = cache.createFromData(asset);
  quoll::InputBinaryStream file(filePath);
  EXPECT_TRUE(file.good());

  quoll::AssetFileHeader header;
  file.read(header);
  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.type, quoll::AssetType::Animation);

  f32 time = 0.0f;
  u32 numKeyframes = 0;

  file.read(time);
  file.read(numKeyframes);

  EXPECT_EQ(time, 5.0f);
  EXPECT_EQ(numKeyframes, 5);

  for (u32 i = 0; i < numKeyframes; ++i) {
    auto &keyframe = asset.data.keyframes.at(i);

    quoll::KeyframeSequenceAssetTarget target{0};
    quoll::KeyframeSequenceAssetInterpolation interpolation{0};
    bool jointTarget = false;
    quoll::JointId joint = 0;
    u32 numValues = 0;

    file.read(target);
    file.read(interpolation);
    file.read(jointTarget);
    file.read(joint);
    file.read(numValues);

    EXPECT_EQ(target, keyframe.target);
    EXPECT_EQ(interpolation, keyframe.interpolation);
    EXPECT_EQ(jointTarget, keyframe.jointTarget);
    EXPECT_EQ(joint, keyframe.joint);
    EXPECT_EQ(numValues, static_cast<u32>(keyframe.keyframeValues.size()));

    std::vector<f32> times(numValues);
    std::vector<glm::vec4> values(numValues);

    file.read(times);
    file.read(values);
    for (u32 i = 0; i < numValues; ++i) {
      EXPECT_EQ(times.at(i), keyframe.keyframeTimes.at(i));
      EXPECT_EQ(values.at(i), keyframe.keyframeValues.at(i));
    }
  }
}

TEST_F(AssetCacheAnimationTest, LoadsAnimationAssetFromFile) {
  auto asset = createRandomizedAnimation();

  auto filePath = cache.createFromData(asset);
  auto res = requestAndWait<quoll::AnimationAsset>(asset.uuid);
  EXPECT_TRUE(res);

  auto animation = res.data();
  EXPECT_TRUE(animation);

  auto &meta = animation.meta();
  EXPECT_EQ(meta.name, asset.name);
  EXPECT_EQ(meta.type, quoll::AssetType::Animation);

  EXPECT_EQ(animation->time, asset.data.time);
  EXPECT_EQ(animation->keyframes.size(), asset.data.keyframes.size());
  for (usize i = 0; i < asset.data.keyframes.size(); ++i) {
    auto &expectedKf = asset.data.keyframes.at(i);
    auto &actualKf = animation->keyframes.at(i);

    EXPECT_EQ(expectedKf.target, actualKf.target);
    EXPECT_EQ(expectedKf.interpolation, actualKf.interpolation);
    EXPECT_EQ(expectedKf.jointTarget, actualKf.jointTarget);
    EXPECT_EQ(expectedKf.joint, actualKf.joint);
    EXPECT_EQ(expectedKf.keyframeTimes.size(), actualKf.keyframeTimes.size());
    EXPECT_EQ(expectedKf.keyframeValues.size(), actualKf.keyframeValues.size());

    for (usize j = 0; j < expectedKf.keyframeTimes.size(); ++j) {
      EXPECT_EQ(expectedKf.keyframeTimes.at(j), actualKf.keyframeTimes.at(j));
      EXPECT_EQ(expectedKf.keyframeValues.at(j), actualKf.keyframeValues.at(j));
    }
  }
}
