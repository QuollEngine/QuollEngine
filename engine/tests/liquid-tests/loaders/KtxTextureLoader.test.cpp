#include "liquid/core/Base.h"
#include "liquid/loaders/KtxTextureLoader.h"
#include "liquid/loaders/KtxError.h"

#include "liquid-tests/Testing.h"

class KtxTextureLoaderTest : public ::testing::Test {
public:
  liquid::rhi::ResourceRegistry registry;
};

using KtxTextureLoaderDeathTest = KtxTextureLoaderTest;

TEST_F(KtxTextureLoaderDeathTest, FailsIfKtxFileCannotBeLoaded) {
  liquid::KtxTextureLoader loader(registry);

  // non-existent file
  EXPECT_DEATH({ loader.loadFromFile("non-existent-file.ktx"); }, ".*");

  // invalid format
  EXPECT_DEATH({ loader.loadFromFile("white-image-100x100.png"); }, ".*");
}

TEST_F(KtxTextureLoaderDeathTest, LoadsTexture1D) {
  liquid::KtxTextureLoader loader(registry);

  EXPECT_DEATH({ loader.loadFromFile("1x1-1d.ktx"); }, ".*");
}

TEST_F(KtxTextureLoaderTest, LoadsTexture2D) {
  liquid::KtxTextureLoader loader(registry);

  auto texture = loader.loadFromFile("1x1-2d.ktx");

  const auto &description = registry.getTextureMap().getDescription(texture);

  EXPECT_EQ(description.type, liquid::rhi::TextureType::Standard);
  EXPECT_EQ(description.width, 1);
  EXPECT_EQ(description.height, 1);
  EXPECT_EQ(description.layers, 1);
  EXPECT_NE(description.data, nullptr);
  EXPECT_EQ(description.format, 43);
}

TEST_F(KtxTextureLoaderTest, LoadsTextureCubemap) {
  liquid::KtxTextureLoader loader(registry);

  auto texture = loader.loadFromFile("1x1-cubemap.ktx");

  const auto &description = registry.getTextureMap().getDescription(texture);

  EXPECT_EQ(description.type, liquid::rhi::TextureType::Cubemap);
  EXPECT_EQ(description.width, 1);
  EXPECT_EQ(description.height, 1);
  EXPECT_EQ(description.layers, 6);
  EXPECT_NE(description.data, nullptr);
  EXPECT_EQ(description.format, 43);
}
