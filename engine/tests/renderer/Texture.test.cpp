#include "core/Base.h"
#include "renderer/Texture.h"

#include "../mocks/TestTextureResourceBinder.h"
#include <gtest/gtest.h>

TEST(TextureTest, StoresMetadataAndResourceBinder) {
  auto binder = std::make_shared<TestTextureResourceBinder>();
  liquid::StatsManager statsManager;

  size_t size = 10;
  uint32_t width = 20;
  uint32_t height = 30;
  uint32_t layers = 1;
  uint32_t format = 1212;
  liquid::Texture texture(binder, size, width, height, layers, format,
                          statsManager);

  EXPECT_EQ(texture.getResourceBinder().get(), binder.get());
  EXPECT_EQ(texture.getSize(), size);
  EXPECT_EQ(texture.getWidth(), width);
  EXPECT_EQ(texture.getHeight(), height);
  EXPECT_EQ(texture.getLayers(), layers);
  EXPECT_EQ(texture.getFormat(), format);
}
