#include <gtest/gtest.h>
#include "renderer/Texture.h"
#include "../mocks/TestTextureResourceBinder.h"

TEST(TextureTest, GetsResourceBinder) {
  auto binder = std::make_shared<TestTextureResourceBinder>();
  liquid::Texture texture(binder, 0, nullptr);

  EXPECT_EQ(texture.getResourceBinder().get(), binder.get());
}
