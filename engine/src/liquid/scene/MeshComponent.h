#pragma once

#include "liquid/core/Base.h"
#include "MeshInstance.h"

namespace liquid {

struct MeshComponent {
  SharedPtr<MeshInstance> instance = nullptr;
};

} // namespace liquid
