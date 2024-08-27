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
  EXPECT_TRUE(filePath);
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.data().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);
  EXPECT_EQ(meta.type, quoll::AssetType::Audio);
  EXPECT_EQ(meta.name, "valid-audio.wav");
}

TEST_F(AssetCacheAudioTest, LoadsWavAudioFileIntoRegistry) {
  auto audioPath = FixturesPath / "valid-audio.wav";

  auto uuid = quoll::Uuid::generate();

  auto filePath = cache.createAudioFromSource(audioPath, uuid);
  auto result = cache.loadAudio(uuid);

  EXPECT_TRUE(result);
  EXPECT_FALSE(result.hasWarnings());

  auto handle = result.data();
  EXPECT_NE(handle, quoll::AssetHandle<quoll::AudioAsset>());
  const auto &asset = cache.getRegistry().getMeta(handle);

  EXPECT_EQ(asset.name, "valid-audio.wav");
  EXPECT_EQ(asset.uuid, uuid);
  EXPECT_EQ(asset.type, quoll::AssetType::Audio);

  EXPECT_EQ(cache.getRegistry().get(handle).format,
            quoll::AudioAssetFormat::Wav);
}

TEST_F(AssetCacheAudioTest, FileReturnsErrorIfAudioFileCannotBeOpened) {
  auto uuid = quoll::Uuid::generate();

  auto result = cache.loadAudio(uuid);
  EXPECT_FALSE(result);
}
