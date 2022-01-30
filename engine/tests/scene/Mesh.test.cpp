#include "liquid/core/Base.h"
#include "liquid/scene/Mesh.h"

#include <gtest/gtest.h>

static bool operator==(const liquid::Vertex &lhs, const liquid::Vertex &rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

TEST(MeshTest, StoresGeometries) {
  liquid::Mesh mesh;
  liquid::Geometry g0, g1, g2;
  liquid::Vertex v0{1.0f, 0.0f, 0.0f}, v1{0.0f, 1.0f, 0.0f},
      v2{0.0f, 0.0f, 1.0f};
  g0.addVertex(v0);
  g1.addVertex(v1);
  g2.addVertex(v2);

  mesh.addGeometry(g0);
  mesh.addGeometry(g1);
  mesh.addGeometry(g2);

  EXPECT_EQ(mesh.getGeometries().size(), 3);
  EXPECT_TRUE(mesh.getGeometries().at(0).getVertices().at(0) == v0);
  EXPECT_TRUE(mesh.getGeometries().at(1).getVertices().at(0) == v1);
  EXPECT_TRUE(mesh.getGeometries().at(2).getVertices().at(0) == v2);
}
