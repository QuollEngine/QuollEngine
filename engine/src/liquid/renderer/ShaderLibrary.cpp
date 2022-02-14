#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "ShaderLibrary.h"

namespace liquid {

void ShaderLibrary::addShader(const String &name,
                              const SharedPtr<Shader> &shader) {
  shaders.insert(std::make_pair(name, shader));

  LOG_DEBUG("Shader \"" << name << "\" loaded");
}

const SharedPtr<Shader> &ShaderLibrary::getShader(const String &name) const {
  const auto &shader = shaders.find(name);
  LIQUID_ASSERT(shader != shaders.end(), "Shader \"" + name + "\" not found");

  return shader->second;
}

} // namespace liquid
