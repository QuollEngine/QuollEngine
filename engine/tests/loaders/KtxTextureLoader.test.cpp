#include "liquid/core/Base.h"
#include <gtest/gtest.h>
#include "../mocks/TestResourceAllocator.h"
#include "liquid/loaders/KtxTextureLoader.h"
#include "liquid/loaders/KtxError.h"

TEST(KtxTextureLoaderDeathTest, FailsIfKtxFileCannotBeLoaded) {
  TestResourceAllocator resourceAllocator;
  liquid::KtxTextureLoader loader(&resourceAllocator);

  // non-existent file
  EXPECT_DEATH({ loader.loadFromFile("non-existent-file.ktx"); }, ".*");

  // invalid format
  EXPECT_DEATH({ loader.loadFromFile("white-image-100x100.png"); }, ".*");
}

TEST(KtxTextureLoaderTests, LoadsTexture2D) {
  TestResourceAllocator resourceAllocator;
  liquid::KtxTextureLoader loader(&resourceAllocator);

  const auto &texture = loader.loadFromFile("1x1-2d.ktx");
  ASSERT_NE(texture, nullptr);

  const auto &binder = std::static_pointer_cast<TestTextureResourceBinder>(
      texture->getResourceBinder());

  EXPECT_EQ(binder->type, TestTextureResourceBinder::Texture2D);
  EXPECT_EQ(binder->width, 1);
  EXPECT_EQ(binder->height, 1);
  EXPECT_NE(binder->data, nullptr);
}

TEST(KtxTextureLoaderTests, LoadsTextureCubemap) {
  TestResourceAllocator resourceAllocator;
  liquid::KtxTextureLoader loader(&resourceAllocator);

  const auto &texture = loader.loadFromFile("1x1-cubemap.ktx");
  ASSERT_NE(texture, nullptr);

  const auto &binder = std::static_pointer_cast<TestTextureResourceBinder>(
      texture->getResourceBinder());

  EXPECT_EQ(binder->type, TestTextureResourceBinder::TextureCubemap);
  EXPECT_EQ(binder->width, 1);
  EXPECT_EQ(binder->height, 1);
  EXPECT_NE(binder->data, nullptr);
}

TEST(KtxTextureLoaderDeathTest, LoadsTexture1D) {
  TestResourceAllocator resourceAllocator;
  liquid::KtxTextureLoader loader(&resourceAllocator);

  EXPECT_DEATH({ loader.loadFromFile("1x1-1d.ktx"); }, ".*");
}
