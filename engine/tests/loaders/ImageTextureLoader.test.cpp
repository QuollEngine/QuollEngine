#include "liquid/core/Base.h"
#include "liquid/loaders/ImageTextureLoader.h"

#include <gtest/gtest.h>

class ImageTextureLoaderTest : public ::testing::Test {
public:
  liquid::experimental::ResourceRegistry registry;
};

using ImageTextureLoaderDeathTest = ImageTextureLoaderTest;

TEST_F(ImageTextureLoaderTest, LoadsImageUsingStb) {
  liquid::ImageTextureLoader loader(registry);
  auto texture = loader.loadFromFile("white-image-100x100.png");

  EXPECT_NE(texture, 0);

  const auto &description = registry.getTextureMap().getDescription(texture);

  EXPECT_EQ(description.width, 100);
  EXPECT_EQ(description.height, 100);
  EXPECT_EQ(description.size, 100 * 100 * 4);
  EXPECT_NE(description.data, nullptr);
}

TEST_F(ImageTextureLoaderDeathTest, ThrowsErrorOnFailedLoad) {
  liquid::ImageTextureLoader loader(registry);
  EXPECT_DEATH(loader.loadFromFile("non-existent-image.png"), ".*");
}
