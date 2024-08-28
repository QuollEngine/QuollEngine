#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/InputBinaryStream.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include <random>

class AssetCacheTextureTest : public AssetCacheTestBase {
public:
};

TEST_F(AssetCacheTextureTest, CreatesTextureFromSource) {
  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "1x1-2d.ktx", uuid);
  EXPECT_TRUE(filePath);
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.data().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Texture);
  EXPECT_EQ(meta.name, "1x1-2d.ktx");
}

TEST_F(AssetCacheTextureTest, CreatesTextureFromAsset) {
  auto uuid = quoll::Uuid::generate();
  auto createdRes = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "1x1-2d.ktx", uuid);
  auto texture = cache.load<quoll::TextureAsset>(uuid);
  auto handle = texture.data();

  auto asset = cache.getRegistry().getMeta(handle);

  cache.getRegistry().remove(handle);

  auto filePath = cache.createFromData(asset);
  EXPECT_TRUE(filePath);
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.data().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);
  EXPECT_EQ(meta.type, quoll::AssetType::Texture);
  EXPECT_EQ(meta.name, "1x1-2d.ktx");
}

TEST_F(AssetCacheTextureTest, LoadsTextureToRegistry) {
  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "1x1-2d.ktx", uuid);

  auto texture = cache.load<quoll::TextureAsset>(uuid);
  auto handle = texture.data();

  auto asset = cache.getRegistry().getMeta(handle);

  EXPECT_EQ(asset.uuid, uuid);
  EXPECT_EQ(asset.name, "1x1-2d.ktx");
  EXPECT_EQ(asset.type, quoll::AssetType::Texture);
}

TEST_F(AssetCacheTextureTest, FailsIfKtxFileCannotBeLoaded) {
  // non-existent file
  EXPECT_FALSE(cache.load<quoll::TextureAsset>(quoll::Uuid::generate()));

  // invalid format
  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "white-image-100x100.png", uuid);

  EXPECT_FALSE(cache.load<quoll::TextureAsset>(uuid));
}

TEST_F(AssetCacheTextureTest, FailsIfTextureIsOneDimensional) {
  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "1x1-1d.ktx", uuid);

  EXPECT_FALSE(cache.load<quoll::TextureAsset>(uuid));
}

TEST_F(AssetCacheTextureTest, LoadsTexture2D) {
  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "1x1-2d.ktx", uuid);

  auto texture = cache.load<quoll::TextureAsset>(uuid);
  EXPECT_TRUE(texture);

  auto &asset = cache.getRegistry().get(texture.data());

  EXPECT_EQ(asset.width, 1);
  EXPECT_EQ(asset.height, 1);
  EXPECT_EQ(asset.layers, 1);
  EXPECT_GT(asset.data.size(), 0);
}

TEST_F(AssetCacheTextureTest, LoadsTextureCubemap) {
  auto uuid = quoll::Uuid::generate();

  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "1x1-cubemap.ktx", uuid);
  auto texture = cache.load<quoll::TextureAsset>(uuid);

  EXPECT_TRUE(texture);

  const auto &asset = cache.getRegistry().get(texture.data());

  EXPECT_EQ(asset.width, 1);
  EXPECT_EQ(asset.height, 1);
  EXPECT_EQ(asset.layers, 6);
  EXPECT_GT(asset.data.size(), 0);
}
