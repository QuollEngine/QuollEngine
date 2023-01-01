#include "liquid/core/Base.h"
#include "liquid/asset/AssetCache.h"

#include "liquid-tests/Testing.h"

class AssetCacheTest : public ::testing::Test {
public:
  AssetCacheTest() : cache(std::filesystem::current_path()) {}

  liquid::AssetCache cache;
};

using AssetCacheDeathTest = AssetCacheTest;

TEST_F(AssetCacheTest, LoadsWavAudioFileIntoRegistry) {
  auto filePath = std::filesystem::current_path() / "valid-audio.wav";

  auto result = cache.loadAudioFromFile(filePath);

  EXPECT_TRUE(result.hasData());
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());

  auto handle = result.getData();
  EXPECT_NE(handle, liquid::AudioAssetHandle::Invalid);
  const auto &asset = cache.getRegistry().getAudios().getAsset(handle);

  EXPECT_EQ(asset.name, "valid-audio.wav");
  EXPECT_EQ(asset.path, filePath);
  EXPECT_EQ(asset.relativePath, "valid-audio.wav");
  EXPECT_EQ(asset.type, liquid::AssetType::Audio);
  EXPECT_EQ(asset.data.format, liquid::AudioAssetFormat::Wav);
}

TEST_F(AssetCacheTest, LoadsMp3AudioFileIntoRegistry) {
  auto filePath = std::filesystem::current_path() / "valid-audio.mp3";

  auto result = cache.loadAudioFromFile(filePath);

  EXPECT_TRUE(result.hasData());
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());

  auto handle = result.getData();
  EXPECT_NE(handle, liquid::AudioAssetHandle::Invalid);
  const auto &asset = cache.getRegistry().getAudios().getAsset(handle);

  EXPECT_EQ(asset.name, "valid-audio.mp3");
  EXPECT_EQ(asset.path, filePath);
  EXPECT_EQ(asset.relativePath, "valid-audio.mp3");
  EXPECT_EQ(asset.type, liquid::AssetType::Audio);
  EXPECT_EQ(asset.data.format, liquid::AudioAssetFormat::Mp3);
}

TEST_F(AssetCacheTest, FileReturnsErrorIfAudioFileHasInvalidExtension) {
  auto filePath = std::filesystem::current_path() / "white-image-100x100.png";

  auto result = cache.loadAudioFromFile(filePath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheTest, FileReturnsErrorIfAudioFileCannotBeOpened) {
  auto filePath = std::filesystem::current_path() / "non-existent-file.wav";

  auto result = cache.loadAudioFromFile(filePath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}
