#include <gtest/gtest.h>
#include "renderer/Texture.h"
#include "../mocks/TestTextureResourceBinder.h"

TEST(TextureTest, GetsResourceBinder) {
  auto binder = std::make_shared<TestTextureResourceBinder>();
  liquid::StatsManager statsManager;
  liquid::Texture texture(binder, 0, statsManager);

  EXPECT_EQ(texture.getResourceBinder().get(), binder.get());
}
