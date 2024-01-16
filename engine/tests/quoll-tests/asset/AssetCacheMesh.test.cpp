#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/AssetFileHeader.h"
#include "quoll/asset/InputBinaryStream.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include <random>

class AssetCacheMeshTest : public AssetCacheTestBase {
public:
  quoll::AssetData<quoll::MeshAsset> createRandomizedMeshAsset() {
    quoll::AssetData<quoll::MeshAsset> asset;
    asset.name = "test-mesh0";
    asset.uuid = quoll::Uuid::generate();
    asset.type = quoll::AssetType::Mesh;

    {
      std::random_device device;
      std::mt19937 mt(device());
      std::uniform_real_distribution<f32> dist(-5.0f, 10.0f);
      std::uniform_int_distribution<u32> udist(0, 20);
      usize countGeometries = 2;
      usize countVertices = 10;
      usize countIndices = 20;

      for (usize i = 0; i < countGeometries; ++i) {
        quoll::BaseGeometryAsset geometry;
        for (usize i = 0; i < countVertices; ++i) {
          geometry.positions.push_back({dist(mt), dist(mt), dist(mt)});
          geometry.normals.push_back({dist(mt), dist(mt), dist(mt)});
          geometry.tangents.push_back({dist(mt), dist(mt), dist(mt), dist(mt)});
          geometry.texCoords0.push_back({dist(mt), dist(mt)});
          geometry.texCoords1.push_back({dist(mt), dist(mt)});
        }

        for (usize i = 0; i < countIndices; ++i) {
          geometry.indices.push_back(udist(mt));
        }

        asset.data.geometries.push_back(geometry);
      }
    }

    return asset;
  }

  quoll::AssetData<quoll::MeshAsset> createRandomizedSkinnedMeshAsset() {
    quoll::AssetData<quoll::MeshAsset> asset;
    asset.name = "test-mesh0";
    asset.uuid = quoll::Uuid::generate();
    asset.type = quoll::AssetType::SkinnedMesh;

    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<f32> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<u32> udist(0, 20);
    usize countGeometries = 2;
    usize countVertices = 10;
    usize countIndices = 20;

    for (usize i = 0; i < countGeometries; ++i) {
      quoll::BaseGeometryAsset geometry;
      for (usize i = 0; i < countVertices; ++i) {
        geometry.positions.push_back({dist(mt), dist(mt), dist(mt)});
        geometry.normals.push_back({dist(mt), dist(mt), dist(mt)});
        geometry.tangents.push_back({dist(mt), dist(mt), dist(mt), dist(mt)});
        geometry.texCoords0.push_back({dist(mt), dist(mt)});
        geometry.texCoords1.push_back({dist(mt), dist(mt)});
        geometry.joints.push_back({udist(mt), udist(mt), udist(mt), udist(mt)});
        geometry.weights.push_back({dist(mt), dist(mt), dist(mt), dist(mt)});
      }

      for (usize i = 0; i < countIndices; ++i) {
        geometry.indices.push_back(udist(mt));
      }

      asset.data.geometries.push_back(geometry);
    }
    return asset;
  }

  void SetUp() override {
    AssetCacheTestBase::SetUp();
    std::filesystem::copy(FixturesPath / "1x1-2d.ktx", CachePath / "tex.asset");
  }
};

TEST_F(AssetCacheMeshTest, CreatesMeshFileFromMeshAsset) {
  auto asset = createRandomizedMeshAsset();
  auto filePath = cache.createMeshFromAsset(asset);

  quoll::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  quoll::AssetFileHeader header;
  file.read(header);
  EXPECT_EQ(header.name, asset.name);
  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.type, quoll::AssetType::Mesh);

  u32 numGeometries = 0;
  file.read(numGeometries);

  EXPECT_EQ(numGeometries, 2);

  for (u32 i = 0; i < numGeometries; ++i) {
    u32 numVertices = 0;
    file.read(numVertices);
    EXPECT_EQ(numVertices, 10);
    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).positions.at(v);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).normals.at(v);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).tangents.at(v);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).texCoords0.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).texCoords1.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    u32 numIndices = 0;
    file.read(numIndices);
    EXPECT_EQ(numIndices, 20);

    for (u32 idx = 0; idx < numIndices; ++idx) {
      const auto valueExpected = asset.data.geometries.at(i).indices.at(idx);
      u32 valueActual = 100000;
      file.read(valueActual);
      EXPECT_EQ(valueExpected, valueActual);
    }
  }

  EXPECT_FALSE(std::filesystem::exists(
      filePath.getData().replace_extension("assetmeta")));
}

TEST_F(AssetCacheMeshTest, DoesNotLoadMeshIfItHasNoVertices) {
  auto asset = createRandomizedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.positions.clear();
  }

  auto filePath = cache.createMeshFromAsset(asset).getData();
  auto handle = cache.loadMesh(asset.uuid);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheMeshTest, DoesNotLoadMeshIfItHasNoIndices) {
  auto asset = createRandomizedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.indices.clear();
  }

  auto filePath = cache.createMeshFromAsset(asset).getData();
  auto handle = cache.loadMesh(asset.uuid);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheMeshTest, LoadsMeshFromFile) {
  auto asset = createRandomizedMeshAsset();
  auto filePath = cache.createMeshFromAsset(asset).getData();
  auto handleRes = cache.loadMesh(asset.uuid);

  auto handle = handleRes.getData();
  EXPECT_NE(handle, quoll::MeshAssetHandle::Null);
  auto &mesh = cache.getRegistry().getMeshes().getAsset(handle);
  EXPECT_EQ(mesh.name, asset.name);

  for (usize g = 0; g < asset.data.geometries.size(); ++g) {
    auto &expectedGeometry = asset.data.geometries.at(g);
    auto &actualGeometry = mesh.data.geometries.at(g);

    EXPECT_EQ(expectedGeometry.positions.size(),
              actualGeometry.positions.size());
    for (usize v = 0; v < expectedGeometry.positions.size(); ++v) {
      auto &e = expectedGeometry;
      auto &a = actualGeometry;

      EXPECT_EQ(e.positions.at(v), a.positions.at(v));
      EXPECT_EQ(e.normals.at(v), a.normals.at(v));
      EXPECT_EQ(e.tangents.at(v), a.tangents.at(v));
      EXPECT_EQ(e.texCoords0.at(v), a.texCoords0.at(v));
      EXPECT_EQ(e.texCoords1.at(v), a.texCoords1.at(v));
    }

    for (usize i = 0; i < expectedGeometry.indices.size(); ++i) {
      auto expected = expectedGeometry.indices.at(i);
      auto actual = actualGeometry.indices.at(i);
      EXPECT_EQ(expected, actual);
    }
  }
}
TEST_F(AssetCacheMeshTest, CreatesSkinnedMeshFileFromSkinnedMeshAsset) {
  auto asset = createRandomizedSkinnedMeshAsset();

  auto filePath = cache.createMeshFromAsset(asset);

  quoll::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  quoll::AssetFileHeader header;
  file.read(header);

  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.name, "test-mesh0");
  EXPECT_EQ(header.type, quoll::AssetType::SkinnedMesh);

  u32 numGeometries = 0;
  file.read(numGeometries);

  EXPECT_EQ(numGeometries, 2);

  for (u32 i = 0; i < numGeometries; ++i) {
    u32 numVertices = 0;
    file.read(numVertices);
    EXPECT_EQ(numVertices, 10);
    for (u32 v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).positions.at(v);
      glm::vec3 valueExpected(vertex.x, vertex.y, vertex.z);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).normals.at(v);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).tangents.at(v);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).texCoords0.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).texCoords1.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).joints.at(v);
      glm::uvec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (u32 v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).weights.at(v);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    u32 numIndices = 0;
    file.read(numIndices);
    EXPECT_EQ(numIndices, 20);

    for (u32 idx = 0; idx < numIndices; ++idx) {
      const auto valueExpected = asset.data.geometries.at(i).indices.at(idx);
      u32 valueActual = 100000;
      file.read(valueActual);
      EXPECT_EQ(valueExpected, valueActual);
    }
  }
}

TEST_F(AssetCacheMeshTest, DoesNotLoadSkinnedMeshIfItHasNoVertices) {
  auto asset = createRandomizedSkinnedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.positions.clear();
  }

  auto filePath = cache.createMeshFromAsset(asset).getData();
  auto handle = cache.loadMesh(asset.uuid);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheMeshTest, DoesNotLoadSkinnedMeshIfItHasNoIndices) {
  auto asset = createRandomizedSkinnedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.indices.clear();
  }

  auto filePath = cache.createMeshFromAsset(asset).getData();
  auto handle = cache.loadMesh(asset.uuid);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheMeshTest, LoadsSkinnedMeshFromFile) {
  auto asset = createRandomizedSkinnedMeshAsset();

  auto filePath = cache.createMeshFromAsset(asset);
  auto handle = cache.loadMesh(asset.uuid);
  EXPECT_NE(handle.getData(), quoll::MeshAssetHandle::Null);
  auto &mesh = cache.getRegistry().getMeshes().getAsset(handle.getData());
  EXPECT_EQ(mesh.name, asset.name);

  for (usize g = 0; g < asset.data.geometries.size(); ++g) {
    auto &expectedGeometry = asset.data.geometries.at(g);
    auto &actualGeometry = mesh.data.geometries.at(g);

    EXPECT_EQ(expectedGeometry.positions.size(),
              actualGeometry.positions.size());
    for (usize v = 0; v < expectedGeometry.positions.size(); ++v) {
      auto &e = expectedGeometry;
      auto &a = actualGeometry;

      EXPECT_EQ(e.positions.at(v), a.positions.at(v));
      EXPECT_EQ(e.normals.at(v), a.normals.at(v));
      EXPECT_EQ(e.tangents.at(v), a.tangents.at(v));
      EXPECT_EQ(e.texCoords0.at(v), a.texCoords0.at(v));
      EXPECT_EQ(e.texCoords1.at(v), a.texCoords1.at(v));
      EXPECT_EQ(e.joints.at(v), a.joints.at(v));
      EXPECT_EQ(e.weights.at(v), a.weights.at(v));
    }

    for (usize i = 0; i < expectedGeometry.indices.size(); ++i) {
      auto expected = expectedGeometry.indices.at(i);
      auto actual = actualGeometry.indices.at(i);
      EXPECT_EQ(expected, actual);
    }
  }
}
