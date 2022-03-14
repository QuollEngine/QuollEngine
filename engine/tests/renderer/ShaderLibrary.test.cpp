#include "liquid/core/Base.h"
#include "liquid/renderer/ShaderLibrary.h"

#include <gtest/gtest.h>

TEST(ShaderLibraryDeathTest, ThrowsErrorIfShaderNotFound) {
  liquid::ShaderLibrary library;

  EXPECT_DEATH({ library.getShader("non-existing-shader"); }, ".*");
}

TEST(ShaderLibraryTests, AddsShader) {
  liquid::ShaderLibrary library;

  library.addShader("shader1", liquid::rhi::ShaderHandle(1));
  library.addShader("shader2", liquid::rhi::ShaderHandle(2));

  EXPECT_EQ(library.getShader("shader1"), liquid::rhi::ShaderHandle(1));
  EXPECT_EQ(library.getShader("shader2"), liquid::rhi::ShaderHandle(2));
}
