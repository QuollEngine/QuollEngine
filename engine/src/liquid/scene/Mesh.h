#pragma once

#include "BaseGeometry.h"
#include "BaseMesh.h"
#include "Vertex.h"

namespace liquid {

using Geometry = BaseGeometry<Vertex>;
using Mesh = BaseMesh<Vertex>;

} // namespace liquid
