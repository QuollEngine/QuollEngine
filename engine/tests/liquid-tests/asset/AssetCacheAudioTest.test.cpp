#include "liquid/core/Base.h"
#include "liquid/asset/AssetCache.h"

#include "liquid-tests/Testing.h"
#include "liquid-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheAudioTest : public AssetCacheTestBase {
public:
};

TEST_F(AssetCacheAudioTest, CreatesAudioFromSource) {
  auto audioPath = FixturesPath / "valid-audio.wav";

  auto filePath =
      cache.createAudioFromSource(audioPath, liquid::Uuid::generate());
  EXPECT_TRUE(filePath.hasData());
  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.getData().filename().string().size(), 38);

  auto meta =
      cache.getMetaFromUuid(liquid::Uuid(filePath.getData().stem().string()));
  EXPECT_EQ(meta.type, liquid::AssetType::Audio);
  EXPECT_EQ(meta.name, "valid-audio.wav");
}

TEST_F(AssetCacheAudioTest, LoadsWavAudioFileIntoRegistry) {
  auto audioPath = FixturesPath / "valid-audio.wav";
  auto filePath =
      cache.createAudioFromSource(audioPath, liquid::Uuid::generate());

  auto result = cache.loadAudioFromFile(filePath.getData());

  EXPECT_TRUE(result.hasData());
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());

  auto handle = result.getData();
  EXPECT_NE(handle, liquid::AudioAssetHandle::Null);
  const auto &asset = cache.getRegistry().getAudios().getAsset(handle);

  EXPECT_EQ(asset.name, "valid-audio.wav");
  EXPECT_EQ(asset.path, filePath.getData());
  EXPECT_EQ(asset.type, liquid::AssetType::Audio);
  EXPECT_EQ(asset.data.format, liquid::AudioAssetFormat::Wav);
}

TEST_F(AssetCacheAudioTest, FileReturnsErrorIfAudioFileCannotBeOpened) {
  auto filePath = CachePath / "non-existent-file.wav";

  auto result = cache.loadAudioFromFile(filePath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}
