#pragma once

#include "Mesh.h"
#include "MeshInstance.h"

namespace liquid {

struct MeshComponent {
  SharedPtr<MeshInstance<Mesh>> instance;
};

} // namespace liquid
