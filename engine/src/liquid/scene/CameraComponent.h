#pragma once

#include "Camera.h"

namespace liquid {

struct CameraComponent {
  SharedPtr<Camera> camera;
};

} // namespace liquid