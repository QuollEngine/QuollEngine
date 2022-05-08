#pragma once

namespace liquid {

struct PerspectiveLensComponent {
  float fovY = 80.0f;
  float near = 0.001f;
  float far = 1000.0f;
  float aspectRatio = 1.0f;
};

} // namespace liquid
