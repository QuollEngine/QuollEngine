#include "core/Base.h"
#include "core/EngineGlobals.h"
#include "ShaderLibrary.h"

namespace liquid {

void ShaderLibrary::addShader(const String &name,
                              const SharedPtr<Shader> &shader) {
  shaders.insert(std::make_pair(name, shader));

  LOG_DEBUG("Shader \"" << name << "\" loaded");
}

const SharedPtr<Shader> &ShaderLibrary::getShader(const String &name) const {
  const auto &shader = shaders.find(name);
  if (shader == shaders.end()) {
    throw std::runtime_error("Shader \"" + name + "\" not found");
  }

  return shader->second;
}

} // namespace liquid
