#pragma once

namespace liquid {

struct EnvironmentComponent {
  SharedPtr<Texture> irradianceMap = nullptr;
  SharedPtr<Texture> specularMap = nullptr;
  SharedPtr<Texture> brdfLUT = nullptr;
};

} // namespace liquid
