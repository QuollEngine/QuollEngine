#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheAudioTest : public AssetCacheTestBase {
public:
};

TEST_F(AssetCacheAudioTest, CreatesAudioFromSource) {
  auto audioPath = FixturesPath / "valid-audio.wav";

  auto uuid = quoll::Uuid::generate();

  auto filePath = cache.createAudioFromSource(audioPath, uuid);
  EXPECT_TRUE(filePath.hasData());
  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.getData().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);
  EXPECT_EQ(meta.type, quoll::AssetType::Audio);
  EXPECT_EQ(meta.name, "valid-audio.wav");
}

TEST_F(AssetCacheAudioTest, LoadsWavAudioFileIntoRegistry) {
  auto audioPath = FixturesPath / "valid-audio.wav";

  auto uuid = quoll::Uuid::generate();

  auto filePath = cache.createAudioFromSource(audioPath, uuid);
  auto result = cache.loadAudio(uuid);

  EXPECT_TRUE(result.hasData());
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());

  auto handle = result.getData();
  EXPECT_NE(handle, quoll::AudioAssetHandle::Null);
  const auto &asset = cache.getRegistry().getAudios().getAsset(handle);

  EXPECT_EQ(asset.name, "valid-audio.wav");
  EXPECT_EQ(asset.path, filePath.getData());
  EXPECT_EQ(asset.type, quoll::AssetType::Audio);
  EXPECT_EQ(asset.data.format, quoll::AudioAssetFormat::Wav);
}

TEST_F(AssetCacheAudioTest, FileReturnsErrorIfAudioFileCannotBeOpened) {
  auto uuid = quoll::Uuid::generate();

  auto result = cache.loadAudio(uuid);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}
