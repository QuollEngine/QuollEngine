#include "liquid/core/Base.h"
#include "liquid/asset/MeshAsset.h"
#include "liquid/scene/MeshInstance.h"

#include <gtest/gtest.h>

class MeshInstanceTest : public ::testing::Test {
public:
  liquid::rhi::ResourceRegistry registry;
  uint32_t testHandle = 2;
};

using MeshInstanceDeathTest = MeshInstanceTest;

TEST_F(MeshInstanceTest, SetsMeshBuffersAndMaterial) {
  liquid::AssetData<liquid::MeshAsset> mesh;
  std::vector<liquid::SharedPtr<liquid::Material>> materials;

  mesh.data.geometries.resize(2);
  mesh.data.geometries.at(0).vertices.resize(20);
  mesh.data.geometries.at(1).vertices.resize(50);
  mesh.data.geometries.at(0).indices.resize(10);

  mesh.data.vertexBuffers.resize(2);
  mesh.data.indexBuffers.resize(2);
  materials.resize(2);

  mesh.data.vertexBuffers.at(0) = liquid::rhi::BufferHandle{2};
  mesh.data.vertexBuffers.at(1) = liquid::rhi::BufferHandle{3};

  mesh.data.indexBuffers.at(0) = liquid::rhi::BufferHandle{4};
  mesh.data.indexBuffers.at(1) = liquid::rhi::BufferHandle::Invalid;

  materials.at(0) = liquid::SharedPtr<liquid::Material>(
      new liquid::Material({}, {}, registry));
  materials.at(1) = liquid::SharedPtr<liquid::Material>(
      new liquid::Material({}, {}, registry));

  liquid::MeshInstance instance(testHandle, mesh, materials);

  EXPECT_EQ(instance.getVertexBuffers().size(), 2);
  EXPECT_EQ(instance.getIndexBuffers().size(), 2);
  EXPECT_EQ(instance.getMaterials().size(), 2);
  EXPECT_EQ(instance.getVertexCounts().size(), 2);
  EXPECT_EQ(instance.getIndexCounts().size(), 2);

  for (size_t i = 0; i < 2; ++i) {
    EXPECT_TRUE(liquid::rhi::isHandleValid(instance.getVertexBuffers().at(i)));
    EXPECT_EQ(instance.getMaterials().at(i).get(), materials.at(i).get());
    EXPECT_EQ(instance.getVertexCounts().at(i),
              mesh.data.geometries.at(i).vertices.size());
    EXPECT_EQ(instance.getIndexCounts().at(i),
              mesh.data.geometries.at(i).indices.size());
  }
  EXPECT_TRUE(liquid::rhi::isHandleValid(instance.getIndexBuffers().at(0)));
  EXPECT_FALSE(liquid::rhi::isHandleValid(instance.getIndexBuffers().at(1)));
}

TEST_F(MeshInstanceTest, SetsSkinnedMeshBuffersAndMaterial) {
  liquid::AssetData<liquid::SkinnedMeshAsset> mesh;
  std::vector<liquid::SharedPtr<liquid::Material>> materials;

  mesh.data.geometries.resize(2);
  mesh.data.geometries.at(0).vertices.resize(20);
  mesh.data.geometries.at(1).vertices.resize(50);
  mesh.data.geometries.at(0).indices.resize(10);

  mesh.data.vertexBuffers.resize(2);
  mesh.data.indexBuffers.resize(2);
  materials.resize(2);

  mesh.data.vertexBuffers.at(0) = liquid::rhi::BufferHandle{2};
  mesh.data.vertexBuffers.at(1) = liquid::rhi::BufferHandle{3};

  mesh.data.indexBuffers.at(0) = liquid::rhi::BufferHandle{4};
  mesh.data.indexBuffers.at(1) = liquid::rhi::BufferHandle::Invalid;

  materials.at(0) = liquid::SharedPtr<liquid::Material>(
      new liquid::Material({}, {}, registry));
  materials.at(1) = liquid::SharedPtr<liquid::Material>(
      new liquid::Material({}, {}, registry));

  liquid::MeshInstance instance(testHandle, mesh, materials);

  EXPECT_EQ(instance.getVertexBuffers().size(), 2);
  EXPECT_EQ(instance.getIndexBuffers().size(), 2);
  EXPECT_EQ(instance.getMaterials().size(), 2);
  EXPECT_EQ(instance.getVertexCounts().size(), 2);
  EXPECT_EQ(instance.getIndexCounts().size(), 2);

  for (size_t i = 0; i < 2; ++i) {
    EXPECT_TRUE(liquid::rhi::isHandleValid(instance.getVertexBuffers().at(i)));
    EXPECT_EQ(instance.getMaterials().at(i).get(), materials.at(i).get());
    EXPECT_EQ(instance.getVertexCounts().at(i),
              mesh.data.geometries.at(i).vertices.size());
    EXPECT_EQ(instance.getIndexCounts().at(i),
              mesh.data.geometries.at(i).indices.size());
  }
  EXPECT_TRUE(liquid::rhi::isHandleValid(instance.getIndexBuffers().at(0)));
  EXPECT_FALSE(liquid::rhi::isHandleValid(instance.getIndexBuffers().at(1)));
}

TEST_F(MeshInstanceDeathTest, FailsToCreateInstanceIfBufferSizesDoNotMatch) {
  liquid::AssetData<liquid::SkinnedMeshAsset> mesh;
  std::vector<liquid::SharedPtr<liquid::Material>> materials;

  mesh.data.vertexBuffers.resize(2);
  mesh.data.indexBuffers.resize(3);
  materials.resize(2);

  EXPECT_DEATH({ liquid::MeshInstance instance(testHandle, mesh, materials); },
               ".*");
}

TEST_F(MeshInstanceDeathTest,
       FailsToCreateInstanceIfMaterialSizeDoesNotMatchBufferSize) {
  liquid::AssetData<liquid::SkinnedMeshAsset> mesh;
  std::vector<liquid::SharedPtr<liquid::Material>> materials;

  mesh.data.vertexBuffers.resize(2);
  mesh.data.indexBuffers.resize(2);
  materials.resize(3);

  EXPECT_DEATH({ liquid::MeshInstance instance(testHandle, mesh, materials); },
               ".*");
}
