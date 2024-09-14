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
    asset.type = quoll::AssetType::Mesh;

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

TEST_F(AssetCacheMeshTest, CreatesMetaFileFromAsset) {
  auto asset = createRandomizedMeshAsset();
  auto filePath = cache.createFromData(asset);
  auto meta = cache.getAssetMeta(asset.uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Mesh);
  EXPECT_EQ(meta.name, "test-mesh0");
}

TEST_F(AssetCacheMeshTest, CreatesMeshFileFromMeshAsset) {
  auto asset = createRandomizedMeshAsset();
  auto filePath = cache.createFromData(asset);

  quoll::InputBinaryStream file(filePath);
  EXPECT_TRUE(file.good());

  quoll::AssetFileHeader header;
  file.read(header);
  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.type, quoll::AssetType::Mesh);

  u32 numGeometries = 0;
  file.read(numGeometries);

  EXPECT_EQ(numGeometries, 2);

  for (u32 i = 0; i < numGeometries; ++i) {
    const auto &g = asset.data.geometries.at(i);

    u32 numAttributes = 0;

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.positions.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.positions.at(v);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.normals.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.normals.at(v);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.tangents.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.tangents.at(v);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.texCoords0.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.texCoords0.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.texCoords1.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.texCoords1.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.joints.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.joints.at(v);
      glm::uvec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.weights.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.weights.at(v);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.indices.size());
    for (u32 idx = 0; idx < numAttributes; ++idx) {
      const auto valueExpected = asset.data.geometries.at(i).indices.at(idx);
      u32 valueActual = 100000;
      file.read(valueActual);
      EXPECT_EQ(valueExpected, valueActual);
    }
  }
}

TEST_F(AssetCacheMeshTest, DoesNotLoadMeshIfItHasNoVertices) {
  auto asset = createRandomizedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.positions.clear();
  }

  auto filePath = cache.createFromData(asset);
  auto mesh = requestAndWait<quoll::MeshAsset>(asset.uuid);
  EXPECT_FALSE(mesh);
}

TEST_F(AssetCacheMeshTest, DoesNotLoadMeshIfItHasNoIndices) {
  auto asset = createRandomizedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.indices.clear();
  }

  auto filePath = cache.createFromData(asset);
  auto mesh = requestAndWait<quoll::MeshAsset>(asset.uuid);
  EXPECT_FALSE(mesh);
}

TEST_F(AssetCacheMeshTest, LoadsMeshFromFile) {
  auto asset = createRandomizedMeshAsset();
  auto filePath = cache.createFromData(asset);
  auto res = requestAndWait<quoll::MeshAsset>(asset.uuid);
  ASSERT_TRUE(res);

  auto mesh = res.data();

  EXPECT_NE(mesh.handle(), quoll::AssetHandle<quoll::MeshAsset>());
  EXPECT_EQ(mesh.meta().name, asset.name);

  for (usize g = 0; g < asset.data.geometries.size(); ++g) {
    auto &expectedGeometry = asset.data.geometries.at(g);
    auto &actualGeometry = mesh->geometries.at(g);

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
TEST_F(AssetCacheMeshTest, CreateMeshAssetWithSkinData) {
  auto asset = createRandomizedSkinnedMeshAsset();

  auto filePath = cache.createFromData(asset);

  quoll::InputBinaryStream file(filePath);
  EXPECT_TRUE(file.good());

  quoll::AssetFileHeader header;
  file.read(header);

  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.type, quoll::AssetType::Mesh);

  u32 numGeometries = 0;
  file.read(numGeometries);

  EXPECT_EQ(numGeometries, 2);

  for (u32 i = 0; i < numGeometries; ++i) {
    auto &g = asset.data.geometries.at(i);
    u32 numAttributes = 0;

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.positions.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.positions.at(v);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.normals.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.normals.at(v);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.tangents.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.tangents.at(v);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.texCoords0.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.texCoords0.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.texCoords1.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.texCoords1.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.joints.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.joints.at(v);
      glm::uvec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.weights.size());
    for (u32 v = 0; v < numAttributes; ++v) {
      const auto &valueExpected = g.weights.at(v);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    file.read(numAttributes);
    EXPECT_EQ(numAttributes, g.indices.size());
    for (u32 idx = 0; idx < numAttributes; ++idx) {
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

  auto filePath = cache.createFromData(asset);
  auto mesh = requestAndWait<quoll::MeshAsset>(asset.uuid);
  EXPECT_FALSE(mesh);
}

TEST_F(AssetCacheMeshTest, DoesNotLoadSkinnedMeshIfItHasNoIndices) {
  auto asset = createRandomizedSkinnedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.indices.clear();
  }

  auto filePath = cache.createFromData(asset);
  auto mesh = requestAndWait<quoll::MeshAsset>(asset.uuid);
  EXPECT_FALSE(mesh);
}

TEST_F(AssetCacheMeshTest, LoadsSkinnedMeshFromFile) {
  auto asset = createRandomizedSkinnedMeshAsset();

  auto filePath = cache.createFromData(asset);
  auto mesh = requestAndWait<quoll::MeshAsset>(asset.uuid).data();
  EXPECT_NE(mesh.handle(), quoll::AssetHandle<quoll::MeshAsset>());
  EXPECT_EQ(mesh.meta().name, asset.name);

  for (usize g = 0; g < asset.data.geometries.size(); ++g) {
    auto &expectedGeometry = asset.data.geometries.at(g);
    auto &actualGeometry = mesh->geometries.at(g);

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
