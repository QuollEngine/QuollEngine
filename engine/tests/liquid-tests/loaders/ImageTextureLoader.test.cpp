#include "liquid/core/Base.h"
#include "liquid/loaders/ImageTextureLoader.h"

#include "liquid-tests/mocks/MockRenderDevice.h"
#include "liquid-tests/Testing.h"

class ImageTextureLoaderTest : public ::testing::Test {
public:
  ImageTextureLoaderTest() : renderStorage(&device) {}
  MockRenderDevice device;
  liquid::RenderStorage renderStorage;
};

using ImageTextureLoaderDeathTest = ImageTextureLoaderTest;

TEST_F(ImageTextureLoaderTest, LoadsImageUsingStb) {
  liquid::ImageTextureLoader loader(renderStorage);

  auto texture = loader.loadFromFile("white-image-100x100.png");

  EXPECT_TRUE(liquid::rhi::isHandleValid(texture));

  const auto &description = device.getTextureDescription(texture);

  EXPECT_EQ(description.width, 100);
  EXPECT_EQ(description.height, 100);
  EXPECT_EQ(description.size, 100 * 100 * 4);
  EXPECT_NE(description.data, nullptr);
}

TEST_F(ImageTextureLoaderDeathTest, ThrowsErrorOnFailedLoad) {
  liquid::ImageTextureLoader loader(renderStorage);

  EXPECT_DEATH(loader.loadFromFile("non-existent-image.png"), ".*");
}
