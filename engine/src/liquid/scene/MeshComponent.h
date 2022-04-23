#pragma once

#include "MeshInstance.h"

namespace liquid {

struct MeshComponent {
  SharedPtr<MeshInstance> instance;
};

} // namespace liquid
