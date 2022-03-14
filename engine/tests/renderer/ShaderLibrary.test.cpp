#include "liquid/core/Base.h"
#include "liquid/renderer/ShaderLibrary.h"

#include <gtest/gtest.h>

TEST(ShaderLibraryDeathTest, ThrowsErrorIfShaderNotFound) {
  liquid::ShaderLibrary library;

  EXPECT_DEATH({ library.getShader("non-existing-shader"); }, ".*");
}

TEST(ShaderLibraryTests, AddsShader) {
  liquid::ShaderLibrary library;

  library.addShader("shader1", 1);
  library.addShader("shader2", 2);

  EXPECT_EQ(library.getShader("shader1"), 1);
  EXPECT_EQ(library.getShader("shader2"), 2);
}
