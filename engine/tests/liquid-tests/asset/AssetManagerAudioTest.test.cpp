#include "liquid/core/Base.h"
#include "liquid/asset/AssetManager.h"

#include "liquid-tests/Testing.h"

class AssetManagerTest : public ::testing::Test {
public:
  AssetManagerTest() : manager(std::filesystem::current_path()) {}

  liquid::AssetManager manager;
};

using AssetManagerDeathTest = AssetManagerTest;

TEST_F(AssetManagerTest, LoadsWavAudioFileIntoRegistry) {
  auto filePath = std::filesystem::current_path() / "valid-audio.wav";

  auto result = manager.loadAudioFromFile(filePath);

  EXPECT_TRUE(result.hasData());
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());

  auto handle = result.getData();
  EXPECT_NE(handle, liquid::AudioAssetHandle::Invalid);
  const auto &asset = manager.getRegistry().getAudios().getAsset(handle);

  EXPECT_EQ(asset.name, "valid-audio.wav");
  EXPECT_EQ(asset.path, filePath);
  EXPECT_EQ(asset.relativePath, "valid-audio.wav");
  EXPECT_EQ(asset.type, liquid::AssetType::Audio);
  EXPECT_EQ(asset.data.format, liquid::AudioAssetFormat::Wav);
}

TEST_F(AssetManagerTest, LoadsMp3AudioFileIntoRegistry) {
  auto filePath = std::filesystem::current_path() / "valid-audio.mp3";

  auto result = manager.loadAudioFromFile(filePath);

  EXPECT_TRUE(result.hasData());
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());

  auto handle = result.getData();
  EXPECT_NE(handle, liquid::AudioAssetHandle::Invalid);
  const auto &asset = manager.getRegistry().getAudios().getAsset(handle);

  EXPECT_EQ(asset.name, "valid-audio.mp3");
  EXPECT_EQ(asset.path, filePath);
  EXPECT_EQ(asset.relativePath, "valid-audio.mp3");
  EXPECT_EQ(asset.type, liquid::AssetType::Audio);
  EXPECT_EQ(asset.data.format, liquid::AudioAssetFormat::Mp3);
}

TEST_F(AssetManagerTest, FileReturnsErrorIfAudioFileHasInvalidExtension) {
  auto filePath = std::filesystem::current_path() / "white-image-100x100.png";

  auto result = manager.loadAudioFromFile(filePath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetManagerTest, FileReturnsErrorIfAudioFileCannotBeOpened) {
  auto filePath = std::filesystem::current_path() / "non-existent-file.wav";

  auto result = manager.loadAudioFromFile(filePath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}
