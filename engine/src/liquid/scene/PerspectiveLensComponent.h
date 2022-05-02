#pragma once

namespace liquid {

struct PerspectiveLensComponent {
  float fovY = 80.0f;
  float near = 0.0f;
  float far = 1000.0f;
  float aspectRatio = 0.0f;
};

} // namespace liquid
