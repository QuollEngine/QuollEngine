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
  auto textureUuid = quoll::Uuid::generate();
  quoll::AssetData<quoll::TextureAsset> textureData{};

  {
    auto createdRes = cache.createFromSource<quoll::TextureAsset>(
        FixturesPath / "1x1-2d.ktx", textureUuid);
    auto res = requestAndWait<quoll::TextureAsset>(textureUuid);
    ASSERT_TRUE(res);
    textureData.uuid = res.data().meta().uuid;
    textureData.name = res.data().meta().name;
    textureData.type = res.data().meta().type;
    textureData.data = res.data().get();
  }

  cache.getRegistry().clear<quoll::TextureAsset>();

  auto filePath = cache.createFromData(textureData);
  EXPECT_TRUE(filePath);
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.data().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(textureUuid);
  EXPECT_EQ(meta.type, quoll::AssetType::Texture);
  EXPECT_EQ(meta.name, "1x1-2d.ktx");
}

TEST_F(AssetCacheTextureTest, LoadsTextureToRegistry) {
  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "1x1-2d.ktx", uuid);

  auto res = requestAndWait<quoll::TextureAsset>(uuid);
  ASSERT_TRUE(res);
  auto texture = res.data();

  EXPECT_EQ(texture.meta().uuid, uuid);
  EXPECT_EQ(texture.meta().name, "1x1-2d.ktx");
  EXPECT_EQ(texture.meta().type, quoll::AssetType::Texture);
}

TEST_F(AssetCacheTextureTest, FailsIfKtxFileCannotBeLoaded) {
  // non-existent file
  EXPECT_FALSE(requestAndWait<quoll::TextureAsset>(quoll::Uuid::generate()));

  // invalid format
  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "white-image-100x100.png", uuid);

  EXPECT_FALSE(requestAndWait<quoll::TextureAsset>(uuid));
}

TEST_F(AssetCacheTextureTest, FailsIfTextureIsOneDimensional) {
  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "1x1-1d.ktx", uuid);

  EXPECT_FALSE(requestAndWait<quoll::TextureAsset>(uuid));
}

TEST_F(AssetCacheTextureTest, LoadsTexture2D) {
  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "1x1-2d.ktx", uuid);

  auto res = requestAndWait<quoll::TextureAsset>(uuid);
  EXPECT_TRUE(res);

  auto texture = res.data();

  EXPECT_EQ(texture->width, 1);
  EXPECT_EQ(texture->height, 1);
  EXPECT_EQ(texture->layers, 1);
  EXPECT_GT(texture->data.size(), 0);
}

TEST_F(AssetCacheTextureTest, LoadsTextureCubemap) {
  auto uuid = quoll::Uuid::generate();

  auto filePath = cache.createFromSource<quoll::TextureAsset>(
      FixturesPath / "1x1-cubemap.ktx", uuid);
  auto res = requestAndWait<quoll::TextureAsset>(uuid);
  EXPECT_TRUE(res);

  auto texture = res.data();

  EXPECT_EQ(texture->width, 1);
  EXPECT_EQ(texture->height, 1);
  EXPECT_EQ(texture->layers, 6);
  EXPECT_GT(texture->data.size(), 0);
}
