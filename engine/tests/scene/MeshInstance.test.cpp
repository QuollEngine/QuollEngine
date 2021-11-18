#include "core/Base.h"
#include "scene/Mesh.h"
#include "scene/MeshInstance.h"

#include "../mocks/TestResourceAllocator.h"
#include "../mocks/TestResourceManager.h"
#include <gtest/gtest.h>

using MaterialPtr = liquid::SharedPtr<liquid::Material>;

liquid::SharedPtr<liquid::Shader> emptyShader(nullptr);

TEST(MeshInstanceTest, CreatesVertexAndIndexBuffersOnConstruct) {
  TestResourceAllocator resourceAllocator;

  liquid::Mesh mesh;
  liquid::Geometry geom1, geom2;

  geom1.addTriangle(0, 1, 2);
  geom2.addTriangle(0, 1, 2);

  mesh.addGeometry(geom1);
  mesh.addGeometry(geom2);

  liquid::MeshInstance instance(&mesh, &resourceAllocator);

  EXPECT_EQ(instance.getVertexBuffers().size(), 2);
  EXPECT_EQ(instance.getIndexBuffers().size(), 2);
  EXPECT_EQ(instance.getMaterials().size(), 2);

  for (size_t i = 0; i < 2; ++i) {
    EXPECT_EQ(instance.getVertexBuffers().at(i)->getType(),
              liquid::HardwareBuffer::Vertex);
    ASSERT_NE(instance.getIndexBuffers().at(i), nullptr);
    EXPECT_EQ(instance.getIndexBuffers().at(i)->getType(),
              liquid::HardwareBuffer::Index);
  }
}

TEST(MeshInstanceTest, CreateWithoutIndexBuffersOnConstruct) {
  TestResourceAllocator resourceAllocator;

  liquid::Mesh mesh;
  liquid::Geometry geom1, geom2;
  geom2.addTriangle(0, 1, 2);

  mesh.addGeometry(geom1);
  mesh.addGeometry(geom2);
  liquid::MeshInstance instance(&mesh, &resourceAllocator);

  EXPECT_EQ(instance.getVertexBuffers().size(), 2);
  EXPECT_EQ(instance.getIndexBuffers().size(), 2);
  EXPECT_EQ(instance.getMaterials().size(), 2);

  for (size_t i = 0; i < 2; ++i) {
    EXPECT_EQ(instance.getVertexBuffers().at(i)->getType(),
              liquid::HardwareBuffer::Vertex);
  }

  EXPECT_EQ(instance.getIndexBuffers().at(0), nullptr);
  EXPECT_EQ(instance.getIndexBuffers().at(1)->getType(),
            liquid::HardwareBuffer::Index);
}

TEST(MeshInstanceTest, SetsMaterial) {
  TestResourceAllocator resourceAllocator;
  TestResourceManager resourceManager;

  liquid::Mesh mesh;
  liquid::Geometry geom;
  mesh.addGeometry(geom);
  MaterialPtr material(new liquid::Material(
      emptyShader, emptyShader, {}, {}, liquid::CullMode::None,
      &resourceAllocator, &resourceManager));
  liquid::MeshInstance instance(&mesh, &resourceAllocator);
  EXPECT_EQ(instance.getMaterials().size(), 1);
  EXPECT_EQ(instance.getMaterials().at(0), nullptr);

  instance.setMaterial(material);
  EXPECT_NE(instance.getMaterials().at(0), nullptr);
}
