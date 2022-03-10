#pragma once

namespace liquid {

struct EnvironmentComponent {
  TextureHandle irradianceMap = 0;
  TextureHandle specularMap = 0;
  TextureHandle brdfLUT = 0;
};

} // namespace liquid
