#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "ShaderLibrary.h"

namespace liquid {

void ShaderLibrary::addShader(StringView name, rhi::ShaderHandle shader) {
  mShaders.insert(std::make_pair(name, shader));

  LOG_DEBUG("Shader \"" << name << "\" loaded");
}

rhi::ShaderHandle ShaderLibrary::getShader(StringView name) const {
  const auto &shader = mShaders.find(String(name));
  LIQUID_ASSERT(shader != mShaders.end(),
                "Shader \"" + String(name) + "\" not found");

  return shader->second;
}

} // namespace liquid
