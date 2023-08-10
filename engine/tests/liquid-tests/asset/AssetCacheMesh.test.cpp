#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"

class AssetCacheTest : public ::testing::Test {
public:
  AssetCacheTest() : cache(FixturesPath) {}

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
        liquid::BaseGeometryAsset<liquid::Vertex> geometry;
        for (size_t i = 0; i < countVertices; ++i) {
          geometry.vertices.push_back({// positions
                                       dist(mt), dist(mt), dist(mt),

                                       // normals
                                       dist(mt), dist(mt), dist(mt),

                                       // colors
                                       dist(mt), dist(mt), dist(mt),

                                       // tangents
                                       dist(mt), dist(mt), dist(mt), dist(mt),

                                       // texcoords0
                                       dist(mt), dist(mt),

                                       // texcoords1
                                       dist(mt), dist(mt)});
        }

        for (size_t i = 0; i < countIndices; ++i) {
          geometry.indices.push_back(udist(mt));
        }

        liquid::AssetData<liquid::MaterialAsset> material;
        material.path =
            liquid::Path(FixturesPath / "materials" /
                         ("material-geom-" + std::to_string(i) + ".lqmat"));

        geometry.material =
            cache.getRegistry().getMaterials().addAsset(material);

        asset.data.geometries.push_back(geometry);
      }
    }

    return asset;
  }

  liquid::AssetData<liquid::SkinnedMeshAsset>
  createRandomizedSkinnedMeshAsset() {
    liquid::AssetData<liquid::SkinnedMeshAsset> asset;
    asset.name = "test-mesh0";

    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<uint32_t> udist(0, 20);
    size_t countGeometries = 2;
    size_t countVertices = 10;
    size_t countIndices = 20;

    for (size_t i = 0; i < countGeometries; ++i) {
      liquid::BaseGeometryAsset<liquid::SkinnedVertex> geometry;
      for (size_t i = 0; i < countVertices; ++i) {
        geometry.vertices.push_back({// positions
                                     dist(mt), dist(mt), dist(mt),

                                     // normals
                                     dist(mt), dist(mt), dist(mt),

                                     // colors
                                     dist(mt), dist(mt), dist(mt),

                                     // tangents
                                     dist(mt), dist(mt), dist(mt), dist(mt),

                                     // texcoords0
                                     dist(mt), dist(mt),

                                     // texcoords1
                                     dist(mt), dist(mt),

                                     // joints
                                     udist(mt), udist(mt), udist(mt), udist(mt),

                                     // weights
                                     dist(mt), dist(mt), dist(mt), dist(mt)});
      }

      for (size_t i = 0; i < countIndices; ++i) {
        geometry.indices.push_back(udist(mt));
      }

      liquid::AssetData<liquid::MaterialAsset> material;
      material.path =
          liquid::Path(FixturesPath / "materials" /
                       ("material-geom-" + std::to_string(i) + ".lqmat"));

      geometry.material = cache.getRegistry().getMaterials().addAsset(material);

      asset.data.geometries.push_back(geometry);
    }
    return asset;
  }

  liquid::AssetCache cache;
};

TEST_F(AssetCacheTest, CreatesMeshFileFromMeshAsset) {
  auto asset = createRandomizedMeshAsset();
  auto filePath = cache.createMeshFromAsset(asset);

  liquid::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::AssetFileMagicLength, '$');
  file.read(magic.data(), magic.length());
  file.read(header.version);
  file.read(header.type);
  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(header.type, liquid::AssetType::Mesh);

  uint32_t numGeometries = 0;
  file.read(numGeometries);

  EXPECT_EQ(numGeometries, 2);

  for (uint32_t i = 0; i < numGeometries; ++i) {
    uint32_t numVertices = 0;
    file.read(numVertices);
    EXPECT_EQ(numVertices, 10);
    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec3 valueExpected(vertex.x, vertex.y, vertex.z);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec3 valueExpected(vertex.nx, vertex.ny, vertex.nz);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec4 valueExpected(vertex.tx, vertex.ty, vertex.tz, vertex.tw);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec2 valueExpected(vertex.u0, vertex.v0);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec2 valueExpected(vertex.u1, vertex.v1);
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

    liquid::String materialPath;
    file.read(materialPath);
    EXPECT_EQ(materialPath,
              "materials/material-geom-" + std::to_string(i) + ".lqmat");
  }

  EXPECT_FALSE(std::filesystem::exists(
      filePath.getData().replace_extension("assetmeta")));
}

TEST_F(AssetCacheTest, DoesNotLoadMeshIfItHasNoVertices) {
  auto asset = createRandomizedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.vertices.clear();
  }

  auto filePath = cache.createMeshFromAsset(asset).getData();
  auto handle = cache.loadMeshFromFile(filePath);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheTest, DoesNotLoadMeshIfItHasNoIndices) {
  auto asset = createRandomizedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.indices.clear();
  }

  auto filePath = cache.createMeshFromAsset(asset).getData();
  auto handle = cache.loadMeshFromFile(filePath);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheTest, LoadsMeshFromFile) {
  auto asset = createRandomizedMeshAsset();
  auto filePath = cache.createMeshFromAsset(asset).getData();
  auto handle = cache.loadMeshFromFile(filePath).getData();
  EXPECT_NE(handle, liquid::MeshAssetHandle::Null);
  auto &mesh = cache.getRegistry().getMeshes().getAsset(handle);

  for (size_t g = 0; g < asset.data.geometries.size(); ++g) {
    auto &expectedGeometry = asset.data.geometries.at(g);
    auto &actualGeometry = mesh.data.geometries.at(g);

    for (size_t v = 0; v < expectedGeometry.vertices.size(); ++v) {
      auto &expected = expectedGeometry.vertices.at(v);
      auto &actual = actualGeometry.vertices.at(v);

      EXPECT_EQ(glm::vec3(expected.x, expected.y, expected.z),
                glm::vec3(actual.x, actual.y, actual.z));
      EXPECT_EQ(glm::vec3(expected.nx, expected.ny, expected.nz),
                glm::vec3(actual.nx, actual.ny, actual.nz));
      EXPECT_EQ(glm::vec4(expected.tx, expected.ty, expected.tz, expected.tw),
                glm::vec4(actual.tx, actual.ty, actual.tz, actual.tw));
      EXPECT_EQ(glm::vec2(expected.u0, expected.v0),
                glm::vec2(actual.u0, actual.v0));
      EXPECT_EQ(glm::vec2(expected.u1, expected.v1),
                glm::vec2(actual.u1, actual.v1));
    }

    for (size_t i = 0; i < expectedGeometry.indices.size(); ++i) {
      auto expected = expectedGeometry.indices.at(i);
      auto actual = actualGeometry.indices.at(i);
      EXPECT_EQ(expected, actual);
    }

    EXPECT_EQ(expectedGeometry.material, actualGeometry.material);
  }
}

TEST_F(AssetCacheTest, LoadsMeshWithMaterials) {
  auto textureHandle = cache.loadTextureFromFile(FixturesPath / "1x1-2d.ktx");
  liquid::AssetData<liquid::MaterialAsset> materialData{};
  materialData.name = "test-mesh-material";
  materialData.data.baseColorTexture = textureHandle.getData();
  auto materialPath = cache.createMaterialFromAsset(materialData);
  auto materialHandle = cache.loadMaterialFromFile(materialPath.getData());

  liquid::AssetData<liquid::MeshAsset> meshData{};
  meshData.name = "test-mesh";

  liquid::BaseGeometryAsset<liquid::Vertex> geometry;
  geometry.material = materialHandle.getData();
  geometry.vertices.push_back({1.0f});
  geometry.indices.push_back(1);
  meshData.data.geometries.push_back(geometry);

  auto meshPath = cache.createMeshFromAsset(meshData);

  cache.getRegistry().getTextures().deleteAsset(textureHandle.getData());
  cache.getRegistry().getMaterials().deleteAsset(materialHandle.getData());

  auto meshHandle = cache.loadMeshFromFile(meshPath.getData()).getData();
  auto &newMesh = cache.getRegistry().getMeshes().getAsset(meshHandle);

  EXPECT_NE(newMesh.data.geometries.at(0).material,
            liquid::MaterialAssetHandle::Null);

  auto &newMaterial = cache.getRegistry().getMaterials().getAsset(
      newMesh.data.geometries.at(0).material);
  EXPECT_NE(newMaterial.data.baseColorTexture,
            liquid::TextureAssetHandle::Null);

  auto &newTexture = cache.getRegistry().getTextures().getAsset(
      newMaterial.data.baseColorTexture);
  EXPECT_EQ(newTexture.name, "1x1-2d.ktx");
}

TEST_F(AssetCacheTest, CreatesSkinnedMeshFileFromSkinnedMeshAsset) {
  auto asset = createRandomizedSkinnedMeshAsset();

  auto filePath = cache.createSkinnedMeshFromAsset(asset);

  liquid::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::AssetFileMagicLength, '$');
  file.read(magic.data(), magic.length());
  file.read(header.version);
  file.read(header.type);
  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(header.type, liquid::AssetType::SkinnedMesh);

  uint32_t numGeometries = 0;
  file.read(numGeometries);

  EXPECT_EQ(numGeometries, 2);

  for (uint32_t i = 0; i < numGeometries; ++i) {
    uint32_t numVertices = 0;
    file.read(numVertices);
    EXPECT_EQ(numVertices, 10);
    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec3 valueExpected(vertex.x, vertex.y, vertex.z);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec3 valueExpected(vertex.nx, vertex.ny, vertex.nz);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec4 valueExpected(vertex.tx, vertex.ty, vertex.tz, vertex.tw);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec2 valueExpected(vertex.u0, vertex.v0);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec2 valueExpected(vertex.u1, vertex.v1);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::uvec4 valueExpected(vertex.j0, vertex.j1, vertex.j2, vertex.j3);
      glm::uvec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec4 valueExpected(vertex.w0, vertex.w1, vertex.w2, vertex.w3);
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

    liquid::String materialPath;
    file.read(materialPath);
    EXPECT_EQ(materialPath,
              "materials/material-geom-" + std::to_string(i) + ".lqmat");
  }
}

TEST_F(AssetCacheTest, DoesNotLoadSkinnedMeshIfItHasNoVertices) {
  auto asset = createRandomizedSkinnedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.vertices.clear();
  }

  auto filePath = cache.createSkinnedMeshFromAsset(asset).getData();
  auto handle = cache.loadSkinnedMeshFromFile(filePath);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheTest, DoesNotLoadSkinnedMeshIfItHasNoIndices) {
  auto asset = createRandomizedSkinnedMeshAsset();
  for (auto &geometry : asset.data.geometries) {
    geometry.indices.clear();
  }

  auto filePath = cache.createSkinnedMeshFromAsset(asset).getData();
  auto handle = cache.loadSkinnedMeshFromFile(filePath);
  EXPECT_TRUE(handle.hasError());
}

TEST_F(AssetCacheTest, LoadsSkinnedMeshFromFile) {
  auto asset = createRandomizedSkinnedMeshAsset();

  auto filePath = cache.createSkinnedMeshFromAsset(asset);
  auto handle = cache.loadSkinnedMeshFromFile(filePath.getData());
  EXPECT_NE(handle.getData(), liquid::SkinnedMeshAssetHandle::Null);
  auto &mesh =
      cache.getRegistry().getSkinnedMeshes().getAsset(handle.getData());

  for (size_t g = 0; g < asset.data.geometries.size(); ++g) {
    auto &expectedGeometry = asset.data.geometries.at(g);
    auto &actualGeometry = mesh.data.geometries.at(g);

    for (size_t v = 0; v < expectedGeometry.vertices.size(); ++v) {
      auto &expected = expectedGeometry.vertices.at(v);
      auto &actual = actualGeometry.vertices.at(v);

      EXPECT_EQ(glm::vec3(expected.x, expected.y, expected.z),
                glm::vec3(actual.x, actual.y, actual.z));
      EXPECT_EQ(glm::vec3(expected.nx, expected.ny, expected.nz),
                glm::vec3(actual.nx, actual.ny, actual.nz));
      EXPECT_EQ(glm::vec4(expected.tx, expected.ty, expected.tz, expected.tw),
                glm::vec4(actual.tx, actual.ty, actual.tz, actual.tw));
      EXPECT_EQ(glm::vec2(expected.u0, expected.v0),
                glm::vec2(actual.u0, actual.v0));
      EXPECT_EQ(glm::vec2(expected.u1, expected.v1),
                glm::vec2(actual.u1, actual.v1));
    }

    for (size_t i = 0; i < expectedGeometry.indices.size(); ++i) {
      auto expected = expectedGeometry.indices.at(i);
      auto actual = actualGeometry.indices.at(i);
      EXPECT_EQ(expected, actual);
    }

    EXPECT_EQ(expectedGeometry.material, actualGeometry.material);
  }
}

TEST_F(AssetCacheTest, LoadsSkinnedMeshWithMaterials) {
  auto textureHandle = cache.loadTextureFromFile(FixturesPath / "1x1-2d.ktx");
  liquid::AssetData<liquid::MaterialAsset> materialData{};
  materialData.name = "test-mesh-material";
  materialData.data.baseColorTexture = textureHandle.getData();
  auto materialPath = cache.createMaterialFromAsset(materialData);
  auto materialHandle = cache.loadMaterialFromFile(materialPath.getData());

  liquid::AssetData<liquid::SkinnedMeshAsset> meshData{};
  meshData.name = "test-smesh";
  liquid::BaseGeometryAsset<liquid::SkinnedVertex> geometry;
  geometry.material = materialHandle.getData();
  geometry.vertices.push_back({1.0f});
  geometry.indices.push_back(1);
  meshData.data.geometries.push_back(geometry);

  auto meshPath = cache.createSkinnedMeshFromAsset(meshData);

  cache.getRegistry().getTextures().deleteAsset(textureHandle.getData());
  cache.getRegistry().getMaterials().deleteAsset(materialHandle.getData());

  auto meshHandle = cache.loadSkinnedMeshFromFile(meshPath.getData()).getData();
  auto &newMesh = cache.getRegistry().getSkinnedMeshes().getAsset(meshHandle);

  EXPECT_NE(newMesh.data.geometries.at(0).material,
            liquid::MaterialAssetHandle::Null);

  auto &newMaterial = cache.getRegistry().getMaterials().getAsset(
      newMesh.data.geometries.at(0).material);
  EXPECT_NE(newMaterial.data.baseColorTexture,
            liquid::TextureAssetHandle::Null);

  auto &newTexture = cache.getRegistry().getTextures().getAsset(
      newMaterial.data.baseColorTexture);
  EXPECT_EQ(newTexture.name, "1x1-2d.ktx");
}
