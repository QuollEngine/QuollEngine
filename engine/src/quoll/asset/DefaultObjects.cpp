#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/core/Uuid.h"
#include "quoll/text/MsdfLoader.h"
#include "AssetData.h"
#include "DefaultObjects.h"

namespace quoll::default_objects {

MeshAsset createCube() {
  // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec4> tangents;
  std::vector<glm::vec2> texCoords;

  std::vector<u32> indices{0,  1,  2,  3,  2,  1,  4,  5,  6,  7,  6,  5,
                           8,  9,  10, 11, 10, 9,  12, 13, 14, 15, 14, 13,
                           16, 17, 18, 19, 18, 17, 20, 21, 22, 23, 22, 21};

  // Front face
  positions.push_back({-1.0f, -1.0f, -1.0f});
  positions.push_back({1.0f, -1.0f, -1.0f});
  positions.push_back({-1.0f, 1.0f, -1.0f});
  positions.push_back({1.0f, 1.0f, -1.0f});

  normals.push_back({0.0, 0.0, -1.0});
  normals.push_back({0.0, 0.0, -1.0});
  normals.push_back({0.0, 0.0, -1.0});
  normals.push_back({0.0, 0.0, -1.0});

  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});

  texCoords.push_back({0.0f, 0.0f});
  texCoords.push_back({1.0f, 0.0f});
  texCoords.push_back({0.0f, 1.0f});
  texCoords.push_back({1.0f, 1.0f});

  // back face (debug = red)
  positions.push_back({-1.0f, 1.0f, 1.0f});
  positions.push_back({1.0f, 1.0f, 1.0f});
  positions.push_back({-1.0f, -1.0f, 1.0f});
  positions.push_back({1.0f, -1.0f, 1.0f});

  normals.push_back({0.0f, 0.0f, 1.0f});
  normals.push_back({0.0f, 0.0f, 1.0f});
  normals.push_back({0.0f, 0.0f, 1.0f});
  normals.push_back({0.0f, 0.0f, 1.0f});

  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});

  texCoords.push_back({0.0f, 0.0f});
  texCoords.push_back({1.0f, 0.0f});
  texCoords.push_back({0.0f, 1.0f});
  texCoords.push_back({1.0f, 1.0f});

  // left face (debug = green)
  positions.push_back({-1.0f, -1.0f, 1.0f});
  positions.push_back({-1.0f, -1.0f, -1.0f});
  positions.push_back({-1.0f, 1.0f, 1.0f});
  positions.push_back({-1.0f, 1.0f, -1.0f});

  normals.push_back({-1.0f, 0.0f, 0.0f});
  normals.push_back({-1.0f, 0.0f, 0.0f});
  normals.push_back({-1.0f, 0.0f, 0.0f});
  normals.push_back({-1.0f, 0.0f, 0.0f});

  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});

  texCoords.push_back({0.0f, 0.0f});
  texCoords.push_back({1.0f, 0.0f});
  texCoords.push_back({0.0f, 1.0f});
  texCoords.push_back({1.0f, 1.0f});

  // right face (debug = yellow)
  positions.push_back({1.0f, -1.0f, -1.0f});
  positions.push_back({1.0f, -1.0f, 1.0f});
  positions.push_back({1.0f, 1.0f, -1.0f});
  positions.push_back({1.0f, 1.0f, 1.0f});

  normals.push_back({1.0f, 0.0f, 0.0f});
  normals.push_back({1.0f, 0.0f, 0.0f});
  normals.push_back({1.0f, 0.0f, 0.0f});
  normals.push_back({1.0f, 0.0f, 0.0f});

  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});

  texCoords.push_back({0.0f, 0.0f});
  texCoords.push_back({1.0f, 0.0f});
  texCoords.push_back({0.0f, 1.0f});
  texCoords.push_back({1.0f, 1.0f});

  // bottom face = cyan
  positions.push_back({-1.0f, 1.0f, -1.0f});
  positions.push_back({1.0f, 1.0f, -1.0f});
  positions.push_back({-1.0f, 1.0f, 1.0f});
  positions.push_back({1.0f, 1.0f, 1.0f});

  normals.push_back({0.0f, 1.0f, 0.0f});
  normals.push_back({0.0f, 1.0f, 0.0f});
  normals.push_back({0.0f, 1.0f, 0.0f});
  normals.push_back({0.0f, 1.0f, 0.0f});

  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});

  texCoords.push_back({0.0f, 0.0f});
  texCoords.push_back({1.0f, 0.0f});
  texCoords.push_back({0.0f, 1.0f});
  texCoords.push_back({1.0f, 1.0f});

  // top face = blue
  positions.push_back({-1.0f, -1.0f, 1.0f});
  positions.push_back({1.0f, -1.0f, 1.0f});
  positions.push_back({-1.0f, -1.0f, -1.0f});
  positions.push_back({1.0f, -1.0f, -1.0f});

  normals.push_back({0.0f, -1.0f, 0.0f});
  normals.push_back({0.0f, -1.0f, 0.0f});
  normals.push_back({0.0f, -1.0f, 0.0f});
  normals.push_back({0.0f, -1.0f, 0.0f});

  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});
  tangents.push_back({0.0f, 0.0f, 0.0f, 0.0f});

  texCoords.push_back({0.0f, 0.0f});
  texCoords.push_back({1.0f, 0.0f});
  texCoords.push_back({0.0f, 1.0f});
  texCoords.push_back({1.0f, 1.0f});

  BaseGeometryAsset geometry;
  geometry.indices = indices;
  geometry.positions = positions;
  geometry.normals = normals;
  geometry.tangents = tangents;
  geometry.texCoords0 = texCoords;
  geometry.texCoords1 = texCoords;

  MeshAsset mesh;
  mesh.geometries.push_back(geometry);
  return mesh;

  // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
}

MaterialAsset createDefaultMaterial() { return {}; }

FontAsset createDefaultFont() {
  MsdfLoader loader;
  auto font =
      loader.loadFontData(Engine::getFontsPath() / "Roboto-Regular.ttf").data();

  return font.data;
}

} // namespace quoll::default_objects
