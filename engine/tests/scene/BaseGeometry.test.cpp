#include "liquid/core/Base.h"
#include "liquid/scene/BaseGeometry.h"
#include "liquid/scene/Vertex.h"

#include <gtest/gtest.h>

static bool operator==(const liquid::Vertex &lhs, const liquid::Vertex &rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

class BaseGeometryTest : public ::testing::Test {
public:
  liquid::experimental::ResourceRegistry registry;
};

TEST_F(BaseGeometryTest, SetsVerticesAndIndicesOnConstruct) {
  const std::vector<liquid::Vertex> vertices{
      {1.0, 2.0, 3.0}, {1.0, 2.0, 3.0}, {1.0, 2.0, 2.0}};
  std::vector<uint32_t> indices{0, 1, 2, 2, 1, 0};
  liquid::SharedPtr<liquid::Material> material(
      new liquid::Material({}, {}, registry));
  liquid::BaseGeometry<liquid::Vertex> geometry(vertices, indices, material);

  auto &meshVertices = geometry.getVertices();
  auto &meshIndices = geometry.getIndices();

  EXPECT_EQ(geometry.getMaterial().get(), material.get());

  for (size_t i = 0; i < vertices.size(); ++i) {
    EXPECT_TRUE(vertices[i] == meshVertices[i]);
  }

  for (size_t i = 0; i < indices.size(); ++i) {
    EXPECT_EQ(indices[i], meshIndices[i]);
  }
}

TEST_F(BaseGeometryTest, AddsTriangleAsIndices) {
  liquid::BaseGeometry<liquid::Vertex> geometry;

  EXPECT_EQ(geometry.getIndices().size(), 0);

  geometry.addTriangle(0, 1, 2);
  geometry.addTriangle(2, 1, 3);

  auto &indices = geometry.getIndices();

  EXPECT_EQ(indices.size(), 6);

  EXPECT_EQ(indices[0], 0);
  EXPECT_EQ(indices[1], 1);
  EXPECT_EQ(indices[2], 2);
  EXPECT_EQ(indices[3], 2);
  EXPECT_EQ(indices[4], 1);
  EXPECT_EQ(indices[5], 3);
}

TEST_F(BaseGeometryTest, AddsVertices) {
  liquid::BaseGeometry<liquid::Vertex> geometry;

  EXPECT_EQ(geometry.getVertices().size(), 0);

  liquid::Vertex v1{0.0, 1.0, 2.0};
  liquid::Vertex v2{2.0, 1.0, 3.0};

  geometry.addVertex(v1);
  geometry.addVertex(v2);

  auto &vertices = geometry.getVertices();

  EXPECT_EQ(vertices.size(), 2);

  EXPECT_TRUE(vertices[0] == v1);
  EXPECT_TRUE(vertices[1] == v2);
}

TEST_F(BaseGeometryTest, SetsMaterial) {
  liquid::BaseGeometry<liquid::Vertex> geometry;
  EXPECT_EQ(geometry.getMaterial().get(), nullptr);

  liquid::SharedPtr<liquid::Material> material(
      new liquid::Material({}, {}, registry));

  geometry.setMaterial(material);
  EXPECT_EQ(geometry.getMaterial().get(), material.get());
}
