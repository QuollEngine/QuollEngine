#include "liquid/core/Base.h"
#include "liquid/asset/AssetManager.h"

#include "liquid-tests/Testing.h"

class AssetManagerTest : public ::testing::Test {
public:
  AssetManagerTest() : manager(std::filesystem::current_path()) {}

  liquid::AssetManager manager;
};

using AssetManagerDeathTest = AssetManagerTest;

TEST_F(AssetManagerTest, LoadsAudioFileIntoRegistry) {
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
}

TEST_F(AssetManagerTest, FileReturnsErrorIfAudioFileCannotBeOpened) {
  auto filePath = std::filesystem::current_path() / "non-existent-file.wav";

  auto result = manager.loadAudioFromFile(filePath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}
