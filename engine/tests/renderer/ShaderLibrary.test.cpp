#include "core/Base.h"
#include "renderer/ShaderLibrary.h"

#include "../mocks/TestShader.h"
#include <gtest/gtest.h>

TEST(ShaderLibraryTests, ThrowsErrorIfShaderNotFound) {
  liquid::ShaderLibrary library;

  EXPECT_THROW({ library.getShader("non-existing-shader"); },
               std::runtime_error);
}

TEST(ShaderLibraryTests, AddsShader) {
  liquid::ShaderLibrary library;
  liquid::SharedPtr<liquid::Shader> shader1 =
      std::make_shared<TestShader>("shader-file-1.shader");
  liquid::SharedPtr<liquid::Shader> shader2 =
      std::make_shared<TestShader>("shader-file-2.shader");
  library.addShader("shader1", shader1);
  library.addShader("shader2", shader1);

  EXPECT_EQ(std::dynamic_pointer_cast<TestShader>(library.getShader("shader1"))
                ->shaderFile,
            "shader-file-1.shader");
  EXPECT_EQ(std::dynamic_pointer_cast<TestShader>(library.getShader("shader2"))
                ->shaderFile,
            "shader-file-1.shader");
}
