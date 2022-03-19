#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "ShaderLibrary.h"

namespace liquid {

void ShaderLibrary::addShader(const String &name, rhi::ShaderHandle shader) {
  mShaders.insert(std::make_pair(name, shader));

  LOG_DEBUG("Shader \"" << name << "\" loaded");
}

rhi::ShaderHandle ShaderLibrary::getShader(const String &name) const {
  const auto &shader = mShaders.find(name);
  LIQUID_ASSERT(shader != mShaders.end(), "Shader \"" + name + "\" not found");

  return shader->second;
}

} // namespace liquid
