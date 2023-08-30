#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"
#include "liquid-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheTextureTest : public AssetCacheTestBase {
public:
};

TEST_F(AssetCacheTextureTest, CreatesTextureFromSource) {
  auto uuid = quoll::Uuid::generate();
  auto filePath =
      cache.createTextureFromSource(FixturesPath / "1x1-2d.ktx", uuid);
  EXPECT_TRUE(filePath.hasData());
  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.getData().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Texture);
  EXPECT_EQ(meta.name, "1x1-2d.ktx");
}

TEST_F(AssetCacheTextureTest, CreatesTextureFromAsset) {
  auto uuid = quoll::Uuid::generate();
  auto createdRes =
      cache.createTextureFromSource(FixturesPath / "1x1-2d.ktx", uuid);
  auto texture = cache.loadTexture(uuid);
  auto handle = texture.getData();

  auto asset = cache.getRegistry().getTextures().getAsset(handle);

  cache.getRegistry().getTextures().deleteAsset(handle);

  auto filePath = cache.createTextureFromAsset(asset);
  EXPECT_TRUE(filePath.hasData());
  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.getData().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);
  EXPECT_EQ(meta.type, quoll::AssetType::Texture);
  EXPECT_EQ(meta.name, "1x1-2d.ktx");
}

TEST_F(AssetCacheTextureTest, LoadsTextureToRegistry) {
  auto uuid = quoll::Uuid::generate();
  auto filePath =
      cache.createTextureFromSource(FixturesPath / "1x1-2d.ktx", uuid);

  auto texture = cache.loadTexture(uuid);
  auto handle = texture.getData();

  auto asset = cache.getRegistry().getTextures().getAsset(handle);

  EXPECT_EQ(asset.path, filePath.getData());
  EXPECT_EQ(asset.name, "1x1-2d.ktx");
  EXPECT_EQ(asset.type, quoll::AssetType::Texture);
}

TEST_F(AssetCacheTextureTest, FailsIfKtxFileCannotBeLoaded) {
  // non-existent file
  EXPECT_TRUE(cache.loadTexture(quoll::Uuid::generate()).hasError());

  // invalid format
  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createTextureFromSource(
      FixturesPath / "white-image-100x100.png", uuid);

  EXPECT_TRUE(cache.loadTexture(uuid).hasError());
}

TEST_F(AssetCacheTextureTest, FailsIfTextureIsOneDimensional) {
  auto uuid = quoll::Uuid::generate();
  auto filePath =
      cache.createTextureFromSource(FixturesPath / "1x1-1d.ktx", uuid);

  EXPECT_TRUE(cache.loadTexture(uuid).hasError());
}

TEST_F(AssetCacheTextureTest, LoadsTexture2D) {
  auto uuid = quoll::Uuid::generate();
  auto filePath =
      cache.createTextureFromSource(FixturesPath / "1x1-2d.ktx", uuid);

  auto texture = cache.loadTexture(uuid);
  EXPECT_TRUE(texture.hasData());

  auto &asset = cache.getRegistry().getTextures().getAsset(texture.getData());

  EXPECT_EQ(asset.data.width, 1);
  EXPECT_EQ(asset.data.height, 1);
  EXPECT_EQ(asset.data.layers, 1);
  EXPECT_GT(asset.data.data.size(), 0);
}

TEST_F(AssetCacheTextureTest, LoadsTextureCubemap) {
  auto uuid = quoll::Uuid::generate();

  auto filePath =
      cache.createTextureFromSource(FixturesPath / "1x1-cubemap.ktx", uuid);
  auto texture = cache.loadTexture(uuid);

  EXPECT_TRUE(texture.hasData());

  const auto &asset =
      cache.getRegistry().getTextures().getAsset(texture.getData());

  EXPECT_EQ(asset.data.width, 1);
  EXPECT_EQ(asset.data.height, 1);
  EXPECT_EQ(asset.data.layers, 6);
  EXPECT_GT(asset.data.data.size(), 0);
}
