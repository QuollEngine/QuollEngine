#pragma once

namespace liquid {

struct EnvironmentComponent {
  rhi::TextureHandle irradianceMap = 0;
  rhi::TextureHandle specularMap = 0;
  rhi::TextureHandle brdfLUT = 0;
};

} // namespace liquid
