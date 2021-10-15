#include "core/Base.h"
#include <gtest/gtest.h>
#include "../mocks/TestResourceAllocator.h"
#include "loaders/KtxTextureLoader.h"
#include "loaders/KtxError.h"

TEST(KtxTextureLoaderTests, FailsIfKtxFileCannotBeLoaded) {
  TestResourceAllocator resourceAllocator;
  liquid::KtxTextureLoader loader(&resourceAllocator);

  // non-existent file
  EXPECT_THROW({ loader.loadFromFile("non-existent-file.ktx"); },
               liquid::KtxError);

  // invalid format
  EXPECT_THROW({ loader.loadFromFile("white-image-100x100.png"); },
               liquid::KtxError);
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

TEST(KtxTextureLoaderTests, LoadsTexture1D) {
  TestResourceAllocator resourceAllocator;
  liquid::KtxTextureLoader loader(&resourceAllocator);

  EXPECT_THROW({ loader.loadFromFile("1x1-1d.ktx"); }, liquid::KtxError);
}
