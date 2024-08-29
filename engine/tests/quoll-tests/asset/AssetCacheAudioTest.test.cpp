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

  auto filePath = cache.createFromSource<quoll::AudioAsset>(audioPath, uuid);
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

  auto filePath = cache.createFromSource<quoll::AudioAsset>(audioPath, uuid);
  auto result = cache.request<quoll::AudioAsset>(uuid);

  EXPECT_TRUE(result);
  EXPECT_FALSE(result.hasWarnings());

  auto asset = result.data();
  EXPECT_NE(asset.handle(), quoll::AssetHandle<quoll::AudioAsset>());

  EXPECT_EQ(asset.meta().name, "valid-audio.wav");
  EXPECT_EQ(asset.meta().uuid, uuid);
  EXPECT_EQ(asset.meta().type, quoll::AssetType::Audio);

  EXPECT_EQ(asset->format, quoll::AudioAssetFormat::Wav);
}

TEST_F(AssetCacheAudioTest, FileReturnsErrorIfAudioFileCannotBeOpened) {
  auto uuid = quoll::Uuid::generate();

  auto result = cache.request<quoll::AudioAsset>(uuid);
  EXPECT_FALSE(result);
}
