#include "liquid/core/Base.h"
#include "liquid/loaders/ImageTextureLoader.h"

#include "../mocks/TestResourceAllocator.h"
#include <gtest/gtest.h>

TEST(ImageTextureLoaderTests, LoadsImageUsingStb) {
  TestResourceAllocator allocator;
  liquid::ImageTextureLoader loader(&allocator);
  const auto &texture = loader.loadFromFile("white-image-100x100.png");

  ASSERT_NE(texture, nullptr);
  const auto &textureData = std::static_pointer_cast<TestTextureResourceBinder>(
      texture->getResourceBinder());

  EXPECT_EQ(textureData->width, 100);
  EXPECT_EQ(textureData->height, 100);
  EXPECT_NE(textureData->data, nullptr);
}

TEST(ImageTextureLoaderTests, ThrowsErrorOnFailedLoad) {
  TestResourceAllocator resourceAllocator;
  liquid::ImageTextureLoader loader(&resourceAllocator);
  EXPECT_THROW(loader.loadFromFile("non-existent-image.png"),
               std::runtime_error);
}
