#pragma once

#include "quoll/rhi/RenderHandle.h"

namespace quoll {

struct BaseGeometryAsset {
  std::vector<glm::vec3> positions;

  std::vector<glm::vec3> normals;

  std::vector<glm::vec4> tangents;

  std::vector<glm::vec2> texCoords0;

  std::vector<glm::vec2> texCoords1;

  std::vector<glm::uvec4> joints;

  std::vector<glm::vec4> weights;

  std::vector<u32> indices;
};

struct MeshAsset {
  std::vector<BaseGeometryAsset> geometries;
};

} // namespace quoll
