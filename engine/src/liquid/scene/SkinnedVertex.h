#pragma once

#include "Vertex.h"

namespace liquid {

struct SkinnedVertex : public Vertex {
  uint32_t j0, j1, j2, j3;
  float w0, w1, w2, w3;
};

} // namespace liquid
