#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include <gtest/gtest.h>
#include <glm/gtx/string_cast.hpp>

class AssetManagerTest : public ::testing::Test {
public:
  AssetManagerTest() : manager(std::filesystem::current_path()) {}

  liquid::AssetManager manager;
};

TEST_F(AssetManagerTest, FailsIfKtxFileCannotBeLoaded) {
  // non-existent file
  EXPECT_TRUE(manager.loadTextureFromFile("non-existent-file.ktx").hasError());

  // invalid format
  EXPECT_TRUE(
      manager.loadTextureFromFile("white-image-100x100.png").hasError());
}

TEST_F(AssetManagerTest, FailsIfTextureIsOneDimensional) {
  EXPECT_TRUE(manager.loadTextureFromFile("1x1-1d.ktx").hasError());
}

TEST_F(AssetManagerTest, LoadsTexture2D) {
  auto texture = manager.loadTextureFromFile("1x1-2d.ktx");
  EXPECT_TRUE(texture.hasData());

  auto &asset = manager.getRegistry().getTextures().getAsset(texture.getData());

  EXPECT_EQ(asset.data.width, 1);
  EXPECT_EQ(asset.data.height, 1);
  EXPECT_EQ(asset.data.layers, 1);
  EXPECT_NE(asset.data.data, nullptr);
}

TEST_F(AssetManagerTest, LoadsTextureCubemap) {
  auto texture = manager.loadTextureFromFile("1x1-cubemap.ktx");
  EXPECT_TRUE(texture.hasData());

  const auto &asset =
      manager.getRegistry().getTextures().getAsset(texture.getData());

  EXPECT_EQ(asset.data.width, 1);
  EXPECT_EQ(asset.data.height, 1);
  EXPECT_EQ(asset.data.layers, 6);
  EXPECT_NE(asset.data.data, nullptr);
}
