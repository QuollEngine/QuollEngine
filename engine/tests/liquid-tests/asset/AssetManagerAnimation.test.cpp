#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"

class AssetManagerTest : public ::testing::Test {
public:
  AssetManagerTest() : manager(std::filesystem::current_path()) {}

  liquid::AssetManager manager;
};

liquid::AssetData<liquid::AnimationAsset> createRandomizedAnimation() {
  liquid::AssetData<liquid::AnimationAsset> asset;
  asset.name = "test-anim0";
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
      liquid::KeyframeSequenceAsset keyframe;
      keyframe.interpolation =
          static_cast<liquid::KeyframeSequenceAssetInterpolation>(
              interpolationDist(mt));
      keyframe.target =
          static_cast<liquid::KeyframeSequenceAssetTarget>(targetDist(mt));
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

TEST_F(AssetManagerTest, CreatesAnimationFile) {
  auto asset = createRandomizedAnimation();
  auto filePath = manager.createAnimationFromAsset(asset);
  liquid::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), magic.length());
  file.read(header.version);
  file.read(header.type);
  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(header.type, liquid::AssetType::Animation);

  float time = 0.0f;
  uint32_t numKeyframes = 0;

  file.read(time);
  file.read(numKeyframes);

  EXPECT_EQ(time, 5.0f);
  EXPECT_EQ(numKeyframes, 5);

  for (uint32_t i = 0; i < numKeyframes; ++i) {
    auto &keyframe = asset.data.keyframes.at(i);

    liquid::KeyframeSequenceAssetTarget target{0};
    liquid::KeyframeSequenceAssetInterpolation interpolation{0};
    bool jointTarget = false;
    liquid::JointId joint = 0;
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
}

TEST_F(AssetManagerTest, LoadsAnimationAssetFromFile) {
  auto asset = createRandomizedAnimation();

  auto filePath = manager.createAnimationFromAsset(asset);
  auto handle = manager.loadAnimationFromFile(filePath.getData());
  EXPECT_FALSE(handle.hasError());
  EXPECT_NE(handle.getData(), liquid::AnimationAssetHandle::Invalid);

  auto &actual =
      manager.getRegistry().getAnimations().getAsset(handle.getData());
  EXPECT_EQ(actual.type, liquid::AssetType::Animation);

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
