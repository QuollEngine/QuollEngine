#pragma once

#include "SkinnedMesh.h"
#include "MeshInstance.h"

namespace liquid {

struct SkinnedMeshComponent {
  SharedPtr<MeshInstance<SkinnedMesh>> instance;
};

} // namespace liquid
