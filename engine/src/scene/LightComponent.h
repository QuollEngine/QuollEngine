#pragma once

#include "scene/Light.h"

namespace liquid {

struct LightComponent {
  SharedPtr<Light> light;
};

} // namespace liquid
