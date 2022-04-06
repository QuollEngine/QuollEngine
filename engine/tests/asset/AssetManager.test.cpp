#include "liquid/core/Base.h"
#include "liquid/asset/AssetManager.h"

#include <gtest/gtest.h>

class AssetManagerTest : public ::testing::Test {
public:
  AssetManagerTest() : manager(std::filesystem::current_path()) {}

  liquid::AssetManager manager;
};

using AssetManagerDeathTest = AssetManagerTest;

TEST_F(AssetManagerDeathTest, FailsIfKtxFileCannotBeLoaded) {
  // non-existent file
  EXPECT_DEATH({ manager.loadTextureFromFile("non-existent-file.ktx"); }, ".*");

  // invalid format
  EXPECT_DEATH({ manager.loadTextureFromFile("white-image-100x100.png"); },
               ".*");
}

TEST_F(AssetManagerDeathTest, FailsIfTextureIsOneDimensional) {
  EXPECT_DEATH({ manager.loadTextureFromFile("1x1-1d.ktx"); }, ".*");
}

TEST_F(AssetManagerTest, LoadsTexture2D) {
  auto texture = manager.loadTextureFromFile("1x1-2d.ktx");

  auto &asset = manager.getRegistry().getTextures().getAsset(texture);

  EXPECT_EQ(asset.data.width, 1);
  EXPECT_EQ(asset.data.height, 1);
  EXPECT_EQ(asset.data.layers, 1);
  EXPECT_NE(asset.data.data, nullptr);
}

TEST_F(AssetManagerTest, LoadsTextureCubemap) {
  auto texture = manager.loadTextureFromFile("1x1-cubemap.ktx");

  const auto &asset = manager.getRegistry().getTextures().getAsset(texture);

  EXPECT_EQ(asset.data.width, 1);
  EXPECT_EQ(asset.data.height, 1);
  EXPECT_EQ(asset.data.layers, 6);
  EXPECT_NE(asset.data.data, nullptr);
}
