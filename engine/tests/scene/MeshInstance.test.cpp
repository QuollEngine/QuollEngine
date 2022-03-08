#include "liquid/core/Base.h"
#include "liquid/scene/Mesh.h"
#include "liquid/scene/MeshInstance.h"

#include <gtest/gtest.h>

using MaterialPtr = liquid::SharedPtr<liquid::Material>;

class MeshInstanceTest : public ::testing::Test {
public:
  liquid::experimental::ResourceRegistry registry;
};

TEST_F(MeshInstanceTest, CreatesVertexAndIndexBuffersOnConstruct) {

  liquid::Mesh mesh;
  liquid::Geometry geom1, geom2;

  geom1.addTriangle(0, 1, 2);
  geom2.addTriangle(0, 1, 2);

  mesh.addGeometry(geom1);
  mesh.addGeometry(geom2);

  liquid::MeshInstance<liquid::Mesh> instance(mesh, registry);

  EXPECT_EQ(instance.getVertexBuffers().size(), 2);
  EXPECT_EQ(instance.getIndexBuffers().size(), 2);
  EXPECT_EQ(instance.getMaterials().size(), 2);

  for (size_t i = 0; i < 2; ++i) {
    EXPECT_NE(instance.getVertexBuffers().at(i), 0);
    EXPECT_NE(instance.getIndexBuffers().at(i), 0);
  }
}

TEST_F(MeshInstanceTest, CreatesWithoutIndexBuffersOnConstruct) {
  liquid::Mesh mesh;
  liquid::Geometry geom1, geom2;
  geom2.addTriangle(0, 1, 2);

  mesh.addGeometry(geom1);
  mesh.addGeometry(geom2);
  liquid::MeshInstance<liquid::Mesh> instance(mesh, registry);

  EXPECT_EQ(instance.getVertexBuffers().size(), 2);
  EXPECT_EQ(instance.getIndexBuffers().size(), 2);
  EXPECT_EQ(instance.getMaterials().size(), 2);

  for (size_t i = 0; i < 2; ++i) {
    EXPECT_NE(instance.getVertexBuffers().at(i), 0);
  }

  EXPECT_EQ(instance.getIndexBuffers().at(0), 0);
  EXPECT_NE(instance.getIndexBuffers().at(1), 0);
}

TEST_F(MeshInstanceTest, SetsMaterial) {
  liquid::Mesh mesh;
  liquid::Geometry geom;
  mesh.addGeometry(geom);
  MaterialPtr material(new liquid::Material({}, {}, registry));
  liquid::MeshInstance<liquid::Mesh> instance(mesh, registry);
  EXPECT_EQ(instance.getMaterials().size(), 1);
  EXPECT_EQ(instance.getMaterials().at(0), nullptr);

  instance.setMaterial(material);
  EXPECT_NE(instance.getMaterials().at(0), nullptr);
}
