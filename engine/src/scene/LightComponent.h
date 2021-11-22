#pragma once

#include "Light.h"

namespace liquid {

struct LightComponent {
  SharedPtr<Light> light;
};

} // namespace liquid
