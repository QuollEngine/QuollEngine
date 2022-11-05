#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"

class AssetCacheTest : public ::testing::Test {
public:
  AssetCacheTest() : manager(std::filesystem::current_path()) {}

  liquid::AssetCache manager;
};

TEST_F(AssetCacheTest, FailsIfKtxFileCannotBeLoaded) {
  // non-existent file
  EXPECT_TRUE(manager.loadTextureFromFile("non-existent-file.ktx").hasError());

  // invalid format
  EXPECT_TRUE(
      manager.loadTextureFromFile("white-image-100x100.png").hasError());
}

TEST_F(AssetCacheTest, FailsIfTextureIsOneDimensional) {
  EXPECT_TRUE(manager.loadTextureFromFile("1x1-1d.ktx").hasError());
}

TEST_F(AssetCacheTest, LoadsTexture2D) {
  auto texture = manager.loadTextureFromFile("1x1-2d.ktx");
  EXPECT_TRUE(texture.hasData());

  auto &asset = manager.getRegistry().getTextures().getAsset(texture.getData());

  EXPECT_EQ(asset.data.width, 1);
  EXPECT_EQ(asset.data.height, 1);
  EXPECT_EQ(asset.data.layers, 1);
  EXPECT_NE(asset.data.data, nullptr);
}

TEST_F(AssetCacheTest, LoadsTextureCubemap) {
  auto texture = manager.loadTextureFromFile("1x1-cubemap.ktx");
  EXPECT_TRUE(texture.hasData());

  const auto &asset =
      manager.getRegistry().getTextures().getAsset(texture.getData());

  EXPECT_EQ(asset.data.width, 1);
  EXPECT_EQ(asset.data.height, 1);
  EXPECT_EQ(asset.data.layers, 6);
  EXPECT_NE(asset.data.data, nullptr);
}
