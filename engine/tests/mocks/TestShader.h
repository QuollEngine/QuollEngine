#pragma once

#include "renderer/Shader.h"

class TestShader : public liquid::Shader {
public:
  TestShader(const liquid::String &shaderFile_) : shaderFile(shaderFile_) {}

public:
  liquid::String shaderFile;
};
