#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"
#include "liquid-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheMeshTest : public AssetCacheTestBase {
public:
  liquid::AssetData<liquid::MeshAsset> createRandomizedMeshAsset() {
    liquid::AssetData<liquid::MeshAsset> asset;
    asset.name = "test-mesh0";

    {
      std::random_device device;
      std::mt19937 mt(device());
      std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
      std::uniform_int_distribution<uint32_t> udist(0, 20);
      size_t countGeometries = 2;
      size_t countVertices = 10;
      size_t countIndices = 20;

      for (size_t i = 0; i < countGeometries; ++i) {
        liquid::BaseGeometryAsset geometry;
        for (size_t i = 0; i < countVertices; ++i) {
          geometry.positions.push_back({dist(mt), dist(mt), dist(mt)});
          geometry.normals.push_back({dist(mt), dist(mt), dist(mt)});
          geometry.tangents.push_back({dist(mt), dist(mt), dist(mt), dist(mt)});
          geometry.texCoords0.push_back({dist(mt), dist(mt)});
          geometry.texCoords1.push_back({dist(mt), dist(mt)});
        }

        for (size_t i = 0; i < countIndices; ++i) {
          geometry.indices.push_back(udist(mt));
        }

        asset.data.geometries.push_back(geometry);
      }
    }

    return asset;
  }

  liquid::AssetData<liquid::MeshAsset> createRandomizedSkinnedMeshAsset() {
    liquid::AssetData<liquid::MeshAsset> asset;
    asset.name = "test-mesh0";

    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<uint32_t> udist(0, 20);
    size_t countGeometries = 2;
    size_t countVertices = 10;
    size_t countIndices = 20;

    for (size_t i = 0; i < countGeometries; ++i) {
      liquid::BaseGeometryAsset geometry;
      for (size_t i = 0; i < countVertices; ++i) {
        geometry.positions.push_back({dist(mt), dist(mt), dist(mt)});
        geometry.normals.push_back({dist(mt), dist(mt), dist(mt)});
        geometry.tangents.push_back({dist(mt), dist(mt), dist(mt), dist(mt)});
        geometry.texCoords0.push_back({dist(mt), dist(mt)});
        geometry.texCoords1.push_back({dist(mt), dist(mt)});
        geometry.joints.push_back({udist(mt), udist(mt), udist(mt), udist(mt)});
        geometry.weights.push_back({dist(mt), dist(mt), dist(mt), dist(mt)});
      }

      for (size_t i = 0; i < countIndices; ++i) {
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
  auto filePath = cache.createMeshFromAsset(asset, "");

  liquid::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  file.read(header);
  EXPECT_EQ(header.name, asset.name);
  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.type, liquid::AssetType::Mesh);

  uint32_t numGeometries = 0;
  file.read(numGeometries);

  EXPECT_EQ(numGeometries, 2);

  for (uint32_t i = 0; i < numGeometries; ++i) {
    uint32_t numVertices = 0;
    file.read(numVertices);
    EXPECT_EQ(numVertices, 10);
    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).positions.at(v);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).normals.at(v);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).tangents.at(v);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).texCoords0.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).texCoords1.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    uint32_t numIndices = 0;
    file.read(numIndices);
    EXPECT_EQ(numIndices, 20);

    for (uint32_t idx = 0; idx < numIndices; ++idx) {
      const auto valueExpected = asset.data.geometries.at(i).indices.at(idx);
      uint32_t valueActual = 100000;
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

  auto filePath = cache.createMeshFromAsset(asset, "").getData();
  auto handle = cache.loadMeshFromFile(filePath);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheMeshTest, DoesNotLoadMeshIfItHasNoIndices) {
  auto asset = createRandomizedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.indices.clear();
  }

  auto filePath = cache.createMeshFromAsset(asset, "").getData();
  auto handle = cache.loadMeshFromFile(filePath);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheMeshTest, LoadsMeshFromFile) {
  auto asset = createRandomizedMeshAsset();
  auto filePath = cache.createMeshFromAsset(asset, "").getData();
  auto handle = cache.loadMeshFromFile(filePath).getData();
  EXPECT_NE(handle, liquid::MeshAssetHandle::Null);
  auto &mesh = cache.getRegistry().getMeshes().getAsset(handle);
  EXPECT_EQ(mesh.name, asset.name);

  for (size_t g = 0; g < asset.data.geometries.size(); ++g) {
    auto &expectedGeometry = asset.data.geometries.at(g);
    auto &actualGeometry = mesh.data.geometries.at(g);

    EXPECT_EQ(expectedGeometry.positions.size(),
              actualGeometry.positions.size());
    for (size_t v = 0; v < expectedGeometry.positions.size(); ++v) {
      auto &e = expectedGeometry;
      auto &a = actualGeometry;

      EXPECT_EQ(e.positions.at(v), a.positions.at(v));
      EXPECT_EQ(e.normals.at(v), a.normals.at(v));
      EXPECT_EQ(e.tangents.at(v), a.tangents.at(v));
      EXPECT_EQ(e.texCoords0.at(v), a.texCoords0.at(v));
      EXPECT_EQ(e.texCoords1.at(v), a.texCoords1.at(v));
    }

    for (size_t i = 0; i < expectedGeometry.indices.size(); ++i) {
      auto expected = expectedGeometry.indices.at(i);
      auto actual = actualGeometry.indices.at(i);
      EXPECT_EQ(expected, actual);
    }
  }
}
TEST_F(AssetCacheMeshTest, CreatesSkinnedMeshFileFromSkinnedMeshAsset) {
  auto asset = createRandomizedSkinnedMeshAsset();

  auto filePath = cache.createSkinnedMeshFromAsset(asset, "");

  liquid::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  file.read(header);

  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.name, "test-mesh0");
  EXPECT_EQ(header.type, liquid::AssetType::SkinnedMesh);

  uint32_t numGeometries = 0;
  file.read(numGeometries);

  EXPECT_EQ(numGeometries, 2);

  for (uint32_t i = 0; i < numGeometries; ++i) {
    uint32_t numVertices = 0;
    file.read(numVertices);
    EXPECT_EQ(numVertices, 10);
    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).positions.at(v);
      glm::vec3 valueExpected(vertex.x, vertex.y, vertex.z);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).normals.at(v);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).tangents.at(v);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).texCoords0.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).texCoords1.at(v);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).joints.at(v);
      glm::uvec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &valueExpected = asset.data.geometries.at(i).weights.at(v);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    uint32_t numIndices = 0;
    file.read(numIndices);
    EXPECT_EQ(numIndices, 20);

    for (uint32_t idx = 0; idx < numIndices; ++idx) {
      const auto valueExpected = asset.data.geometries.at(i).indices.at(idx);
      uint32_t valueActual = 100000;
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

  auto filePath = cache.createSkinnedMeshFromAsset(asset, "").getData();
  auto handle = cache.loadSkinnedMeshFromFile(filePath);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheMeshTest, DoesNotLoadSkinnedMeshIfItHasNoIndices) {
  auto asset = createRandomizedSkinnedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.indices.clear();
  }

  auto filePath = cache.createSkinnedMeshFromAsset(asset, "").getData();
  auto handle = cache.loadSkinnedMeshFromFile(filePath);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheMeshTest, LoadsSkinnedMeshFromFile) {
  auto asset = createRandomizedSkinnedMeshAsset();

  auto filePath = cache.createSkinnedMeshFromAsset(asset, "");
  auto handle = cache.loadSkinnedMeshFromFile(filePath.getData());
  EXPECT_NE(handle.getData(), liquid::SkinnedMeshAssetHandle::Null);
  auto &mesh =
      cache.getRegistry().getSkinnedMeshes().getAsset(handle.getData());
  EXPECT_EQ(mesh.name, asset.name);

  for (size_t g = 0; g < asset.data.geometries.size(); ++g) {
    auto &expectedGeometry = asset.data.geometries.at(g);
    auto &actualGeometry = mesh.data.geometries.at(g);

    EXPECT_EQ(expectedGeometry.positions.size(),
              actualGeometry.positions.size());
    for (size_t v = 0; v < expectedGeometry.positions.size(); ++v) {
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

    for (size_t i = 0; i < expectedGeometry.indices.size(); ++i) {
      auto expected = expectedGeometry.indices.at(i);
      auto actual = actualGeometry.indices.at(i);
      EXPECT_EQ(expected, actual);
    }
  }
}
