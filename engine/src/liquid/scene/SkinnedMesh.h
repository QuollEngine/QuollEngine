#pragma once

#include "BaseGeometry.h"
#include "BaseMesh.h"
#include "SkinnedVertex.h"

namespace liquid {

using SkinnedGeometry = BaseGeometry<SkinnedVertex>;
using SkinnedMesh = BaseMesh<SkinnedVertex>;

} // namespace liquid
