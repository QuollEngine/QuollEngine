#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include <gtest/gtest.h>
#include <glm/gtx/string_cast.hpp>

class AssetManagerTest : public ::testing::Test {
public:
  AssetManagerTest() : manager(std::filesystem::current_path()) {}

  liquid::AssetManager manager;
};

using AssetManagerDeathTest = AssetManagerTest;

TEST_F(AssetManagerDeathTest, FailsIfKtxFileCannotBeLoaded) {
  // non-existent file
  EXPECT_DEATH({ manager.loadTextureFromFile("non-existent-file.ktx"); }, ".*");

  // invalid format
  EXPECT_DEATH({ manager.loadTextureFromFile("white-image-100x100.png"); },
               ".*");
}

TEST_F(AssetManagerDeathTest, FailsIfTextureIsOneDimensional) {
  EXPECT_DEATH({ manager.loadTextureFromFile("1x1-1d.ktx"); }, ".*");
}

TEST_F(AssetManagerTest, LoadsTexture2D) {
  auto texture = manager.loadTextureFromFile("1x1-2d.ktx");

  auto &asset = manager.getRegistry().getTextures().getAsset(texture);

  EXPECT_EQ(asset.data.width, 1);
  EXPECT_EQ(asset.data.height, 1);
  EXPECT_EQ(asset.data.layers, 1);
  EXPECT_NE(asset.data.data, nullptr);
}

TEST_F(AssetManagerTest, LoadsTextureCubemap) {
  auto texture = manager.loadTextureFromFile("1x1-cubemap.ktx");

  const auto &asset = manager.getRegistry().getTextures().getAsset(texture);

  EXPECT_EQ(asset.data.width, 1);
  EXPECT_EQ(asset.data.height, 1);
  EXPECT_EQ(asset.data.layers, 6);
  EXPECT_NE(asset.data.data, nullptr);
}

TEST_F(AssetManagerTest, CreatesMaterialWithTexturesFromAsset) {
  liquid::AssetData<liquid::MaterialAsset> asset{};
  asset.name = "material1";
  asset.type = liquid::AssetType::Material;
  asset.data.baseColorFactor = glm::vec4(2.5f, 0.2f, 0.5f, 5.2f);
  asset.data.baseColorTextureCoord = 2;

  {
    liquid::AssetData<liquid::TextureAsset> texture;
    texture.path = std::filesystem::path(std::filesystem::current_path() /
                                         "textures" / "test.ktx2");
    asset.data.baseColorTexture =
        manager.getRegistry().getTextures().addAsset(texture);
  }

  asset.data.metallicFactor = 1.0f;
  asset.data.roughnessFactor = 2.5f;
  asset.data.metallicRoughnessTextureCoord = 3;
  {
    liquid::AssetData<liquid::TextureAsset> texture;
    texture.path = std::filesystem::path(std::filesystem::current_path() /
                                         "textures" / "mr.ktx2");
    asset.data.metallicRoughnessTexture =
        manager.getRegistry().getTextures().addAsset(texture);
  }

  asset.data.normalScale = 0.6f;
  asset.data.normalTextureCoord = 4;
  {
    liquid::AssetData<liquid::TextureAsset> texture;
    texture.path = std::filesystem::path(std::filesystem::current_path() /
                                         "textures" / "normal.ktx2");
    asset.data.normalTexture =
        manager.getRegistry().getTextures().addAsset(texture);
  }

  asset.data.occlusionStrength = 0.4f;
  asset.data.occlusionTextureCoord = 5;
  {
    liquid::AssetData<liquid::TextureAsset> texture;
    texture.path = std::filesystem::path(std::filesystem::current_path() /
                                         "textures" / "occlusion.ktx2");
    asset.data.occlusionTexture =
        manager.getRegistry().getTextures().addAsset(texture);
  }

  asset.data.emissiveFactor = glm::vec3(0.5f, 0.6f, 2.5f);
  asset.data.emissiveTextureCoord = 6;
  {
    liquid::AssetData<liquid::TextureAsset> texture;
    texture.path = std::filesystem::path(std::filesystem::current_path() /
                                         "textures" / "emissive.ktx2");
    asset.data.emissiveTexture =
        manager.getRegistry().getTextures().addAsset(texture);
  }

  auto assetFile = manager.createMaterialFromAsset(asset);

  {
    liquid::InputBinaryStream file(assetFile);
    EXPECT_TRUE(file.good());

    liquid::AssetFileHeader header;

    liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
    file.read(magic.data(), magic.size());

    file.read(header.version);
    file.read(header.type);

    // Base color
    liquid::String baseTexturePath;
    file.read(baseTexturePath);
    int8_t baseTextureCoord = -1;
    file.read(baseTextureCoord);
    glm::vec4 baseColorFactor;
    file.read(baseColorFactor);

    // Metallic roughness
    liquid::String metallicRoughnessTexturePath;
    file.read(metallicRoughnessTexturePath);
    int8_t metallicRoughnessTextureCoord = -1;
    file.read(metallicRoughnessTextureCoord);
    float metallicFactor = 0.0f;
    file.read(metallicFactor);
    float roughnessFactor = 0.0f;
    file.read(roughnessFactor);

    // Normal
    liquid::String normalTexturePath;
    file.read(normalTexturePath);
    int8_t normalTextureCoord = -1;
    file.read(normalTextureCoord);
    float normalScale = 0.0f;
    file.read(normalScale);

    // Occlusion
    liquid::String occlusionTexturePath;
    file.read(occlusionTexturePath);
    int8_t occlusionTextureCoord = -1;
    file.read(occlusionTextureCoord);
    float occlusionStrength = 0.0f;
    file.read(occlusionStrength);

    // Emissive
    liquid::String emissiveTexturePath;
    file.read(emissiveTexturePath);
    int8_t emissiveTextureCoord = -1;
    file.read(emissiveTextureCoord);
    glm::vec3 emissiveFactor;
    file.read(emissiveFactor);

    EXPECT_EQ(magic, header.magic);
    EXPECT_EQ(header.type, liquid::AssetType::Material);
    EXPECT_EQ(header.version, liquid::createVersion(0, 1));
    EXPECT_EQ(baseTexturePath, "textures/test.ktx2");
    EXPECT_EQ(baseTextureCoord, 2);
    EXPECT_EQ(baseColorFactor, glm::vec4(2.5f, 0.2f, 0.5f, 5.2f));
    EXPECT_EQ(metallicRoughnessTexturePath, "textures/mr.ktx2");
    EXPECT_EQ(metallicRoughnessTextureCoord, 3);
    EXPECT_EQ(metallicFactor, 1.0f);
    EXPECT_EQ(roughnessFactor, 2.5f);
    EXPECT_EQ(normalTexturePath, "textures/normal.ktx2");
    EXPECT_EQ(normalTextureCoord, 4);
    EXPECT_EQ(normalScale, 0.6f);
    EXPECT_EQ(occlusionTexturePath, "textures/occlusion.ktx2");
    EXPECT_EQ(occlusionTextureCoord, 5);
    EXPECT_EQ(occlusionStrength, 0.4f);
    EXPECT_EQ(emissiveTexturePath, "textures/emissive.ktx2");
    EXPECT_EQ(emissiveTextureCoord, 6);
    EXPECT_EQ(emissiveFactor, glm::vec3(0.5f, 0.6f, 2.5f));
  }
  std::filesystem::remove(assetFile);
}

TEST_F(AssetManagerTest,
       CreatesMaterialWithoutTexturesFromAssetIfReferencedTexturesAreInvalid) {
  liquid::AssetData<liquid::MaterialAsset> asset{};
  asset.name = "material2";
  asset.type = liquid::AssetType::Material;
  asset.data.baseColorFactor = glm::vec4(2.5f, 0.2f, 0.5f, 5.2f);
  asset.data.baseColorTextureCoord = 2;
  asset.data.metallicFactor = 1.0f;
  asset.data.roughnessFactor = 2.5f;
  asset.data.metallicRoughnessTextureCoord = 3;
  asset.data.normalScale = 0.6f;
  asset.data.normalTextureCoord = 4;
  asset.data.occlusionStrength = 0.4f;
  asset.data.occlusionTextureCoord = 5;
  asset.data.emissiveFactor = glm::vec3(0.5f, 0.6f, 2.5f);
  asset.data.emissiveTextureCoord = 6;

  auto assetFile = manager.createMaterialFromAsset(asset);

  {
    liquid::InputBinaryStream file(assetFile);
    EXPECT_TRUE(file.good());

    liquid::AssetFileHeader header;
    liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
    file.read(magic.data(), magic.length());
    file.read(header.version);
    file.read(header.type);

    // Base color
    liquid::String baseTexturePath;
    file.read(baseTexturePath);
    int8_t baseTextureCoord = -1;
    file.read(baseTextureCoord);
    glm::vec4 baseColorFactor;
    file.read(baseColorFactor);

    // Metallic roughness
    liquid::String metallicRoughnessTexturePath;
    file.read(metallicRoughnessTexturePath);
    int8_t metallicRoughnessTextureCoord = -1;
    file.read(metallicRoughnessTextureCoord);
    float metallicFactor = 0.0f;
    file.read(metallicFactor);
    float roughnessFactor = 0.0f;
    file.read(roughnessFactor);

    // Normal
    liquid::String normalTexturePath;
    file.read(normalTexturePath);
    int8_t normalTextureCoord = -1;
    file.read(normalTextureCoord);
    float normalScale = 0.0f;
    file.read(normalScale);

    // Occlusion
    liquid::String occlusionTexturePath;
    file.read(occlusionTexturePath);
    int8_t occlusionTextureCoord = -1;
    file.read(occlusionTextureCoord);
    float occlusionStrength = 0.0f;
    file.read(occlusionStrength);

    // Emissive
    liquid::String emissiveTexturePath;
    file.read(emissiveTexturePath);
    int8_t emissiveTextureCoord = -1;
    file.read(emissiveTextureCoord);
    glm::vec3 emissiveFactor;
    file.read(emissiveFactor);

    EXPECT_EQ(magic, header.magic);
    EXPECT_EQ(header.type, liquid::AssetType::Material);
    EXPECT_EQ(header.version, liquid::createVersion(0, 1));
    EXPECT_EQ(baseTexturePath, "");
    EXPECT_EQ(baseTextureCoord, 2);
    EXPECT_EQ(baseColorFactor, glm::vec4(2.5f, 0.2f, 0.5f, 5.2f));
    EXPECT_EQ(metallicRoughnessTexturePath, "");
    EXPECT_EQ(metallicRoughnessTextureCoord, 3);
    EXPECT_EQ(metallicFactor, 1.0f);
    EXPECT_EQ(roughnessFactor, 2.5f);
    EXPECT_EQ(normalTexturePath, "");
    EXPECT_EQ(normalTextureCoord, 4);
    EXPECT_EQ(normalScale, 0.6f);
    EXPECT_EQ(occlusionTexturePath, "");
    EXPECT_EQ(occlusionTextureCoord, 5);
    EXPECT_EQ(occlusionStrength, 0.4f);
    EXPECT_EQ(emissiveTexturePath, "");
    EXPECT_EQ(emissiveTextureCoord, 6);
    EXPECT_EQ(emissiveFactor, glm::vec3(0.5f, 0.6f, 2.5f));
  }

  std::filesystem::remove(assetFile);
}

TEST_F(AssetManagerTest, LoadsMaterialWithTexturesFromFile) {
  liquid::AssetData<liquid::MaterialAsset> asset{};
  asset.name = "material1";
  asset.type = liquid::AssetType::Material;
  asset.data.baseColorFactor = glm::vec4(2.5f, 0.2f, 0.5f, 5.2f);
  asset.data.baseColorTextureCoord = 2;

  {
    liquid::AssetData<liquid::TextureAsset> texture;
    texture.path = std::filesystem::path(std::filesystem::current_path() /
                                         "textures" / "test.ktx2");
    asset.data.baseColorTexture =
        manager.getRegistry().getTextures().addAsset(texture);
  }

  asset.data.metallicFactor = 1.0f;
  asset.data.roughnessFactor = 2.5f;
  asset.data.metallicRoughnessTextureCoord = 3;
  {
    liquid::AssetData<liquid::TextureAsset> texture;
    texture.path = std::filesystem::path(std::filesystem::current_path() /
                                         "textures" / "mr.ktx2");
    asset.data.metallicRoughnessTexture =
        manager.getRegistry().getTextures().addAsset(texture);
  }

  asset.data.normalScale = 0.6f;
  asset.data.normalTextureCoord = 4;
  {
    liquid::AssetData<liquid::TextureAsset> texture;
    texture.path = std::filesystem::path(std::filesystem::current_path() /
                                         "textures" / "normal.ktx2");
    asset.data.normalTexture =
        manager.getRegistry().getTextures().addAsset(texture);
  }

  asset.data.occlusionStrength = 0.4f;
  asset.data.occlusionTextureCoord = 5;
  {
    liquid::AssetData<liquid::TextureAsset> texture;
    texture.path = std::filesystem::path(std::filesystem::current_path() /
                                         "textures" / "occlusion.ktx2");
    asset.data.occlusionTexture =
        manager.getRegistry().getTextures().addAsset(texture);
  }

  asset.data.emissiveFactor = glm::vec3(0.5f, 0.6f, 2.5f);
  asset.data.emissiveTextureCoord = 6;
  {
    liquid::AssetData<liquid::TextureAsset> texture;
    texture.path = std::filesystem::path(std::filesystem::current_path() /
                                         "textures" / "emissive.ktx2");
    asset.data.emissiveTexture =
        manager.getRegistry().getTextures().addAsset(texture);
  }

  auto assetFile = manager.createMaterialFromAsset(asset);

  auto handle = manager.loadMaterialFromFile(assetFile);

  EXPECT_NE(handle, liquid::MaterialAssetHandle::Invalid);

  auto &material = manager.getRegistry().getMaterials().getAsset(handle);
  EXPECT_EQ(material.name, "material1.lqmat");
  EXPECT_EQ(material.path, std::filesystem::current_path() / "material1.lqmat");
  EXPECT_EQ(material.type, liquid::AssetType::Material);

  EXPECT_EQ(material.data.baseColorTexture, asset.data.baseColorTexture);
  EXPECT_EQ(material.data.baseColorTextureCoord,
            asset.data.baseColorTextureCoord);
  EXPECT_EQ(material.data.baseColorFactor, asset.data.baseColorFactor);

  EXPECT_EQ(material.data.metallicRoughnessTexture,
            asset.data.metallicRoughnessTexture);
  EXPECT_EQ(material.data.metallicRoughnessTextureCoord,
            asset.data.metallicRoughnessTextureCoord);
  EXPECT_EQ(material.data.metallicFactor, asset.data.metallicFactor);
  EXPECT_EQ(material.data.roughnessFactor, asset.data.roughnessFactor);

  EXPECT_EQ(material.data.normalTexture, asset.data.normalTexture);
  EXPECT_EQ(material.data.normalTextureCoord, asset.data.normalTextureCoord);
  EXPECT_EQ(material.data.normalScale, asset.data.normalScale);

  EXPECT_EQ(material.data.occlusionTexture, asset.data.occlusionTexture);
  EXPECT_EQ(material.data.occlusionTextureCoord,
            asset.data.occlusionTextureCoord);
  EXPECT_EQ(material.data.occlusionStrength, asset.data.occlusionStrength);

  EXPECT_EQ(material.data.emissiveTexture, asset.data.emissiveTexture);
  EXPECT_EQ(material.data.emissiveTextureCoord,
            asset.data.emissiveTextureCoord);
  EXPECT_EQ(material.data.emissiveFactor, asset.data.emissiveFactor);

  std::filesystem::remove(assetFile);
}

TEST_F(AssetManagerTest, CreatesMeshFileFromMeshAsset) {
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
      material.path = std::filesystem::path(
          std::filesystem::current_path() / "materials" /
          ("material-geom-" + std::to_string(i) + ".lqmat"));

      geometry.material =
          manager.getRegistry().getMaterials().addAsset(material);

      asset.data.geometries.push_back(geometry);
    }
  }

  auto filePath = manager.createMeshFromAsset(asset);

  liquid::InputBinaryStream file(filePath);
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
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

      EXPECT_EQ(valueExpected, valueActual)
          << "Position"
          << "(" << i << ", " << v << "): " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec3 valueExpected(vertex.nx, vertex.ny, vertex.nz);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual)
          << "Normal: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec4 valueExpected(vertex.tx, vertex.ty, vertex.tz, vertex.tw);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual)
          << "Tangent: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec2 valueExpected(vertex.u0, vertex.v0);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual)
          << "TexCoord0: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec2 valueExpected(vertex.u1, vertex.v1);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual)
          << "TexCoord1: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
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

TEST_F(AssetManagerTest, LoadsMeshFromFile) {
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
      material.path = std::filesystem::path(
          std::filesystem::current_path() / "materials" /
          ("material-geom-" + std::to_string(i) + ".lqmat"));

      geometry.material =
          manager.getRegistry().getMaterials().addAsset(material);

      asset.data.geometries.push_back(geometry);
    }
  }

  auto filePath = manager.createMeshFromAsset(asset);
  auto handle = manager.loadMeshFromFile(filePath);
  EXPECT_NE(handle, liquid::MeshAssetHandle::Invalid);
  auto &mesh = manager.getRegistry().getMeshes().getAsset(handle);

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

TEST_F(AssetManagerTest, CreatesSkinnedMeshFileFromSkinnedMeshAsset) {
  liquid::AssetData<liquid::SkinnedMeshAsset> asset;
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
      material.path = std::filesystem::path(
          std::filesystem::current_path() / "materials" /
          ("material-geom-" + std::to_string(i) + ".lqmat"));

      geometry.material =
          manager.getRegistry().getMaterials().addAsset(material);

      asset.data.geometries.push_back(geometry);
    }
  }

  auto filePath = manager.createSkinnedMeshFromAsset(asset);

  liquid::InputBinaryStream file(filePath);
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
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

      EXPECT_EQ(valueExpected, valueActual)
          << "Position: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec3 valueExpected(vertex.nx, vertex.ny, vertex.nz);
      glm::vec3 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual)
          << "Normal: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec4 valueExpected(vertex.tx, vertex.ty, vertex.tz, vertex.tw);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual)
          << "Tangent: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec2 valueExpected(vertex.u0, vertex.v0);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual)
          << "TexCoord0: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec2 valueExpected(vertex.u1, vertex.v1);
      glm::vec2 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual)
          << "TexCoord1: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::uvec4 valueExpected(vertex.j0, vertex.j1, vertex.j2, vertex.j3);
      glm::uvec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual)
          << "Joints: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
    }

    for (uint32_t v = 0; v < numVertices; ++v) {
      const auto &vertex = asset.data.geometries.at(i).vertices.at(v);
      glm::vec4 valueExpected(vertex.w0, vertex.w1, vertex.w2, vertex.w3);
      glm::vec4 valueActual;
      file.read(valueActual);

      EXPECT_EQ(valueExpected, valueActual)
          << "Weights: " << glm::to_string(valueExpected)
          << " != " << glm::to_string(valueActual);
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

TEST_F(AssetManagerTest, LoadsSkinnedMeshFromFile) {
  liquid::AssetData<liquid::SkinnedMeshAsset> asset;
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
      material.path = std::filesystem::path(
          std::filesystem::current_path() / "materials" /
          ("material-geom-" + std::to_string(i) + ".lqmat"));

      geometry.material =
          manager.getRegistry().getMaterials().addAsset(material);

      asset.data.geometries.push_back(geometry);
    }
  }

  auto filePath = manager.createSkinnedMeshFromAsset(asset);
  auto handle = manager.loadSkinnedMeshFromFile(filePath);
  EXPECT_NE(handle, liquid::SkinnedMeshAssetHandle::Invalid);
  auto &mesh = manager.getRegistry().getSkinnedMeshes().getAsset(handle);

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

TEST_F(AssetManagerTest, CreatesSkeletonFileFromSkeletonAsset) {
  liquid::AssetData<liquid::SkeletonAsset> asset;
  asset.name = "test-skel0";

  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<uint32_t> udist(0, 20);

    size_t countJoints = 20;
    for (size_t i = 0; i < countJoints; ++i) {
      asset.data.jointLocalPositions.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalRotations.push_back(
          glm::quat(dist(mt), dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalScales.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointInverseBindMatrices.push_back(glm::mat4{
          // row 0
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
          // row 1
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row 2
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row3
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
      });

      asset.data.jointParents.push_back(udist(mt));
      asset.data.jointNames.push_back("Joint " + std::to_string(i));
    }
  }

  auto filePath = manager.createSkeletonFromAsset(asset);

  liquid::InputBinaryStream file(filePath);
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), magic.length());
  file.read(header.version);
  file.read(header.type);
  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(header.type, liquid::AssetType::Skeleton);

  uint32_t numJoints = 0;
  file.read(numJoints);
  EXPECT_EQ(numJoints, 20);

  std::vector<glm::vec3> actualPositions(numJoints);
  std::vector<glm::quat> actualRotations(numJoints);
  std::vector<glm::vec3> actualScales(numJoints);
  std::vector<glm::mat4> actualInverseBindMatrices(numJoints);
  std::vector<liquid::JointId> actualParents(numJoints);
  std::vector<liquid::String> actualNames(numJoints);
  file.read(actualPositions);
  file.read(actualRotations);
  file.read(actualScales);
  file.read(actualParents);
  file.read(actualInverseBindMatrices);
  file.read(actualNames);

  for (uint32_t i = 0; i < numJoints; ++i) {
    EXPECT_EQ(actualPositions.at(i), asset.data.jointLocalPositions.at(i));
    EXPECT_EQ(actualRotations.at(i), asset.data.jointLocalRotations.at(i));
    EXPECT_EQ(actualScales.at(i), asset.data.jointLocalScales.at(i));
    EXPECT_EQ(actualParents.at(i), asset.data.jointParents.at(i));
    EXPECT_EQ(actualInverseBindMatrices.at(i),
              asset.data.jointInverseBindMatrices.at(i));
    EXPECT_EQ(actualNames.at(i), asset.data.jointNames.at(i));
  }
}

TEST_F(AssetManagerTest, LoadsSkeletonAssetFromFile) {
  liquid::AssetData<liquid::SkeletonAsset> asset;
  asset.name = "test-skel0";
  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<uint32_t> udist(0, 20);

    size_t countJoints = 20;
    for (size_t i = 0; i < countJoints; ++i) {
      asset.data.jointLocalPositions.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalRotations.push_back(
          glm::quat(dist(mt), dist(mt), dist(mt), dist(mt)));
      asset.data.jointLocalScales.push_back(
          glm::vec3(dist(mt), dist(mt), dist(mt)));
      asset.data.jointInverseBindMatrices.push_back(glm::mat4{
          // row 0
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
          // row 1
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row 2
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),

          // row3
          dist(mt),
          dist(mt),
          dist(mt),
          dist(mt),
      });

      asset.data.jointParents.push_back(udist(mt));
      asset.data.jointNames.push_back("Joint " + std::to_string(i));
    }
  }

  auto filePath = manager.createSkeletonFromAsset(asset);
  auto handle = manager.loadSkeletonFromFile(filePath);

  EXPECT_NE(handle, liquid::SkeletonAssetHandle::Invalid);

  auto &actual = manager.getRegistry().getSkeletons().getAsset(handle);

  for (size_t i = 0; i < actual.data.jointLocalPositions.size(); ++i) {
    EXPECT_EQ(actual.data.jointLocalPositions.at(i),
              asset.data.jointLocalPositions.at(i));
    EXPECT_EQ(actual.data.jointLocalRotations.at(i),
              asset.data.jointLocalRotations.at(i));
    EXPECT_EQ(actual.data.jointLocalScales.at(i),
              asset.data.jointLocalScales.at(i));
    EXPECT_EQ(actual.data.jointParents.at(i), asset.data.jointParents.at(i));
    EXPECT_EQ(actual.data.jointInverseBindMatrices.at(i),
              asset.data.jointInverseBindMatrices.at(i));
    EXPECT_EQ(actual.data.jointNames.at(i), asset.data.jointNames.at(i));
  }
}

TEST_F(AssetManagerTest, CreatesAnimationFile) {
  liquid::AssetData<liquid::AnimationAsset> asset;
  asset.name = "test-anim0";
  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<uint32_t> udist(0, 20);
    std::uniform_int_distribution<uint32_t> targetDist(0, 2);
    std::uniform_int_distribution<uint32_t> interpolationDist(0, 1);

    size_t countKeyframes = 5;
    size_t countKeyframeValues = 10;
    asset.data.time = static_cast<float>(countKeyframeValues) * 0.5f;

    for (size_t i = 0; i < countKeyframes; ++i) {
      liquid::KeyframeSequenceAsset keyframe;
      keyframe.interpolation =
          static_cast<liquid::KeyframeSequenceAssetInterpolation>(
              interpolationDist(mt));
      keyframe.target =
          static_cast<liquid::KeyframeSequenceAssetTarget>(targetDist(mt));
      keyframe.joint = udist(mt);
      keyframe.jointTarget = keyframe.joint == 10;
      keyframe.keyframeTimes.resize(countKeyframeValues);
      keyframe.keyframeValues.resize(countKeyframeValues);

      for (size_t j = 0; j < countKeyframeValues; ++j) {
        keyframe.keyframeTimes.at(j) = 0.5f * static_cast<float>(j);
        keyframe.keyframeValues.at(j) =
            glm::vec4(dist(mt), dist(mt), dist(mt), dist(mt));
      }

      asset.data.keyframes.push_back(keyframe);
    }
  }
  auto filePath = manager.createAnimationFromAsset(asset);
  liquid::InputBinaryStream file(filePath);
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), magic.length());
  file.read(header.version);
  file.read(header.type);
  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(header.type, liquid::AssetType::Animation);

  float time = 0.0f;
  uint32_t numKeyframes = 0;

  file.read(time);
  file.read(numKeyframes);

  EXPECT_EQ(time, 5.0f);
  EXPECT_EQ(numKeyframes, 5);

  for (uint32_t i = 0; i < numKeyframes; ++i) {
    auto &keyframe = asset.data.keyframes.at(i);

    liquid::KeyframeSequenceAssetTarget target{0};
    liquid::KeyframeSequenceAssetInterpolation interpolation{0};
    bool jointTarget = false;
    liquid::JointId joint = 0;
    uint32_t numValues = 0;

    file.read(target);
    file.read(interpolation);
    file.read(jointTarget);
    file.read(joint);
    file.read(numValues);

    EXPECT_EQ(target, keyframe.target);
    EXPECT_EQ(interpolation, keyframe.interpolation);
    EXPECT_EQ(jointTarget, keyframe.jointTarget);
    EXPECT_EQ(joint, keyframe.joint);
    EXPECT_EQ(numValues, static_cast<uint32_t>(keyframe.keyframeValues.size()));

    std::vector<float> times(numValues);
    std::vector<glm::vec4> values(numValues);

    file.read(times);
    file.read(values);
    for (uint32_t i = 0; i < numValues; ++i) {
      EXPECT_EQ(times.at(i), keyframe.keyframeTimes.at(i));
      EXPECT_EQ(values.at(i), keyframe.keyframeValues.at(i));
    }
  }
}

TEST_F(AssetManagerTest, LoadsAnimationAssetFromFile) {
  liquid::AssetData<liquid::AnimationAsset> asset;
  asset.name = "test-anim0";
  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<uint32_t> udist(0, 20);
    std::uniform_int_distribution<uint32_t> targetDist(0, 2);
    std::uniform_int_distribution<uint32_t> interpolationDist(0, 1);

    size_t countKeyframes = 5;
    size_t countKeyframeValues = 10;
    asset.data.time = static_cast<float>(countKeyframeValues) * 0.5f;

    for (size_t i = 0; i < countKeyframes; ++i) {
      liquid::KeyframeSequenceAsset keyframe;
      keyframe.interpolation =
          static_cast<liquid::KeyframeSequenceAssetInterpolation>(
              interpolationDist(mt));
      keyframe.target =
          static_cast<liquid::KeyframeSequenceAssetTarget>(targetDist(mt));
      keyframe.joint = udist(mt);
      keyframe.jointTarget = keyframe.joint == 10;
      keyframe.keyframeTimes.resize(countKeyframeValues);
      keyframe.keyframeValues.resize(countKeyframeValues);

      for (size_t j = 0; j < countKeyframeValues; ++j) {
        keyframe.keyframeTimes.at(j) = 0.5f * static_cast<float>(j);
        keyframe.keyframeValues.at(j) =
            glm::vec4(dist(mt), dist(mt), dist(mt), dist(mt));
      }

      asset.data.keyframes.push_back(keyframe);
    }
  }
  auto filePath = manager.createAnimationFromAsset(asset);
  auto handle = manager.loadAnimationFromFile(filePath);
  EXPECT_NE(handle, liquid::AnimationAssetHandle::Invalid);

  auto &actual = manager.getRegistry().getAnimations().getAsset(handle);
  EXPECT_EQ(actual.type, liquid::AssetType::Animation);

  EXPECT_EQ(actual.data.time, asset.data.time);
  EXPECT_EQ(actual.data.keyframes.size(), asset.data.keyframes.size());
  for (size_t i = 0; i < asset.data.keyframes.size(); ++i) {
    auto &expectedKf = asset.data.keyframes.at(i);
    auto &actualKf = actual.data.keyframes.at(i);

    EXPECT_EQ(expectedKf.target, actualKf.target);
    EXPECT_EQ(expectedKf.interpolation, actualKf.interpolation);
    EXPECT_EQ(expectedKf.jointTarget, actualKf.jointTarget);
    EXPECT_EQ(expectedKf.joint, actualKf.joint);
    EXPECT_EQ(expectedKf.keyframeTimes.size(), actualKf.keyframeTimes.size());
    EXPECT_EQ(expectedKf.keyframeValues.size(), actualKf.keyframeValues.size());

    for (size_t j = 0; j < expectedKf.keyframeTimes.size(); ++j) {
      EXPECT_EQ(expectedKf.keyframeTimes.at(j), actualKf.keyframeTimes.at(j));
      EXPECT_EQ(expectedKf.keyframeValues.at(j), actualKf.keyframeValues.at(j));
    }
  }
}

TEST_F(AssetManagerTest, CreatesPrefabFile) {
  liquid::AssetData<liquid::PrefabAsset> asset;
  asset.name = "test-prefab0";

  {
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_real_distribution<float> dist(-5.0f, 10.0f);
    std::uniform_int_distribution<uint32_t> udist(0, 20);
    std::uniform_int_distribution<uint32_t> targetDist(0, 2);
    std::uniform_int_distribution<uint32_t> interpolationDist(0, 1);

    uint32_t numMeshes = 5;
    uint32_t numAnimators = 4;
    uint32_t numAnimationsPerAnimator = 3;
    uint32_t numSkeletons = 3;
    for (uint32_t i = 0; i < numMeshes; ++i) {
      liquid::AssetData<liquid::MeshAsset> mesh;
      mesh.path = std::filesystem::current_path() /
                  ("meshes/mesh-" + std::to_string(i) + ".lqmesh");
      auto handle = manager.getRegistry().getMeshes().addAsset(mesh);
      asset.data.meshes.push_back({i, handle});
    }

    for (uint32_t i = 0; i < numSkeletons; ++i) {
      liquid::AssetData<liquid::SkinnedMeshAsset> mesh;
      mesh.path = std::filesystem::current_path() /
                  ("meshes/smesh-" + std::to_string(i) + ".lqmesh");
      auto handle = manager.getRegistry().getSkinnedMeshes().addAsset(mesh);
      asset.data.skinnedMeshes.push_back({i, handle});
    }

    for (uint32_t i = 0; i < numSkeletons; ++i) {
      liquid::AssetData<liquid::SkeletonAsset> skeleton;
      skeleton.path = std::filesystem::current_path() /
                      ("skeletons/skel-" + std::to_string(i) + ".lqskel");
      auto handle = manager.getRegistry().getSkeletons().addAsset(skeleton);
      asset.data.skeletons.push_back({i, handle});
    }

    asset.data.animators.resize(numAnimators);
    for (uint32_t i = 0; i < numAnimators * numAnimationsPerAnimator; ++i) {
      liquid::AssetData<liquid::AnimationAsset> animation;
      animation.path = std::filesystem::current_path() /
                       ("skeletons/anim-" + std::to_string(i) + ".lqanim");
      auto handle = manager.getRegistry().getAnimations().addAsset(animation);

      uint32_t entityId = (i / numAnimationsPerAnimator);

      asset.data.animators.at(entityId).entity = entityId;
      asset.data.animators.at(entityId).value.animations.push_back(handle);
    }
  }
  auto filePath = manager.createPrefabFromAsset(asset);
  liquid::InputBinaryStream file(filePath);
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), magic.length());
  file.read(header.version);
  file.read(header.type);
  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(header.type, liquid::AssetType::Prefab);

  {
    auto &expected = asset.data.meshes;
    auto &map = manager.getRegistry().getMeshes();
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, static_cast<uint32_t>(expected.size()));
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto expectedString =
          std::filesystem::relative(map.getAsset(expected.at(i).value).path,
                                    std::filesystem::current_path())
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  {
    auto &expected = asset.data.skinnedMeshes;
    auto &map = manager.getRegistry().getSkinnedMeshes();
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, static_cast<uint32_t>(expected.size()));
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto expectedString =
          std::filesystem::relative(map.getAsset(expected.at(i).value).path,
                                    std::filesystem::current_path())
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  {
    auto &expected = asset.data.skeletons;
    auto &map = manager.getRegistry().getSkeletons();
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, static_cast<uint32_t>(expected.size()));
    std::vector<liquid::String> actual(numAssets);
    file.read(actual);

    for (uint32_t i = 0; i < numAssets; ++i) {
      auto expectedString =
          std::filesystem::relative(map.getAsset(expected.at(i).value).path,
                                    std::filesystem::current_path())
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  std::vector<liquid::String> actualAnimations;
  {
    auto &expected = asset.data.animators;
    auto &map = manager.getRegistry().getAnimations();
    auto &actual = actualAnimations;
    uint32_t numAssets = 0;
    file.read(numAssets);
    EXPECT_EQ(numAssets, 12);
    actualAnimations.resize(numAssets);
    file.read(actual);

    for (uint32_t i = 0; i < numAssets; ++i) {
      uint32_t entityId = (i / 3);
      auto animation = expected.at(entityId).value.animations.at(i % 3);

      auto expectedString =
          std::filesystem::relative(map.getAsset(animation).path,
                                    std::filesystem::current_path())
              .string();
      std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

      EXPECT_EQ(expectedString, actual.at(i));
    }
  }

  uint32_t dummy = 0;
  file.read(dummy);
  EXPECT_EQ(dummy, 0);
  file.read(dummy);
  EXPECT_EQ(dummy, 0);
  file.read(dummy);
  EXPECT_EQ(dummy, 0);
  file.read(dummy);

  {
    uint32_t numComponents = 0;
    file.read(numComponents);
    EXPECT_EQ(numComponents, 4);
    auto &map = manager.getRegistry().getAnimations();

    for (uint32_t i = 0; i < numComponents; ++i) {
      uint32_t entity = 999;
      file.read(entity);
      EXPECT_EQ(entity, i);

      uint32_t numAnimations = 0;
      file.read(numAnimations);
      EXPECT_EQ(numAnimations, 3);

      std::vector<uint32_t> animations(numAnimations);
      file.read(animations);

      EXPECT_EQ(asset.data.animators.at(i).value.animations.size(),
                animations.size());

      for (size_t j = 0; j < animations.size(); ++j) {
        auto &expected =
            map.getAsset(asset.data.animators.at(i).value.animations.at(j));

        auto expectedString =
            std::filesystem::relative(expected.path,
                                      std::filesystem::current_path())
                .string();
        std::replace(expectedString.begin(), expectedString.end(), '\\', '/');

        EXPECT_EQ(expectedString, actualAnimations.at(animations.at(j)));
      }
    }
  }
}

TEST_F(AssetManagerTest, LoadsPrefabFile) {
  liquid::AssetData<liquid::PrefabAsset> asset;
  asset.name = "test-prefab0";

  {
    uint32_t numMeshes = 5;
    uint32_t numAnimators = 4;
    uint32_t numAnimationsPerAnimator = 3;
    uint32_t numSkeletons = 3;
    for (uint32_t i = 0; i < numMeshes; ++i) {
      liquid::AssetData<liquid::MeshAsset> mesh;
      mesh.path = std::filesystem::current_path() /
                  ("meshes/mesh-" + std::to_string(i) + ".lqmesh");
      auto handle = manager.getRegistry().getMeshes().addAsset(mesh);
      asset.data.meshes.push_back({i, handle});
    }

    for (uint32_t i = 0; i < numSkeletons; ++i) {
      liquid::AssetData<liquid::SkinnedMeshAsset> mesh;
      mesh.path = std::filesystem::current_path() /
                  ("meshes/smesh-" + std::to_string(i) + ".lqmesh");
      auto handle = manager.getRegistry().getSkinnedMeshes().addAsset(mesh);
      asset.data.skinnedMeshes.push_back({i, handle});
    }

    for (uint32_t i = 0; i < numSkeletons; ++i) {
      liquid::AssetData<liquid::SkeletonAsset> skeleton;
      skeleton.path = std::filesystem::current_path() /
                      ("skeletons/skel-" + std::to_string(i) + ".lqskel");
      auto handle = manager.getRegistry().getSkeletons().addAsset(skeleton);
      asset.data.skeletons.push_back({i, handle});
    }

    asset.data.animators.resize(numAnimators);
    for (uint32_t i = 0; i < numAnimators * numAnimationsPerAnimator; ++i) {
      liquid::AssetData<liquid::AnimationAsset> animation;
      animation.path = std::filesystem::current_path() /
                       ("skeletons/anim-" + std::to_string(i) + ".lqanim");
      auto handle = manager.getRegistry().getAnimations().addAsset(animation);

      uint32_t entityId = (i / numAnimationsPerAnimator);

      asset.data.animators.at(entityId).entity = entityId;
      asset.data.animators.at(entityId).value.animations.push_back(handle);
    }
  }
  auto filePath = manager.createPrefabFromAsset(asset);
  auto handle = manager.loadPrefabFromFile(filePath);
  EXPECT_NE(handle, liquid::PrefabAssetHandle::Invalid);

  auto &prefab = manager.getRegistry().getPrefabs().getAsset(handle);

  EXPECT_EQ(asset.data.meshes.size(), prefab.data.meshes.size());
  for (size_t i = 0; i < prefab.data.meshes.size(); ++i) {
    auto expected = asset.data.meshes.at(i).value;
    auto actual = prefab.data.meshes.at(i).value;
    EXPECT_EQ(expected, actual);
  }

  EXPECT_EQ(asset.data.skinnedMeshes.size(), prefab.data.skinnedMeshes.size());
  for (size_t i = 0; i < prefab.data.skinnedMeshes.size(); ++i) {
    auto expected = asset.data.skinnedMeshes.at(i).value;
    auto actual = prefab.data.skinnedMeshes.at(i).value;
    EXPECT_EQ(expected, actual);
  }

  EXPECT_EQ(asset.data.skeletons.size(), prefab.data.skeletons.size());
  for (size_t i = 0; i < prefab.data.skeletons.size(); ++i) {
    auto expected = asset.data.skeletons.at(i).value;
    auto actual = prefab.data.skeletons.at(i).value;
    EXPECT_EQ(expected, actual);
  }

  EXPECT_EQ(asset.data.animators.size(), prefab.data.animators.size());
  for (size_t i = 0; i < prefab.data.animators.size(); ++i) {
    auto expected = asset.data.animators.at(i).value;
    auto actual = prefab.data.animators.at(i).value;

    EXPECT_EQ(expected.animations.size(), actual.animations.size());

    for (size_t j = 0; j < expected.animations.size(); ++j) {
      EXPECT_EQ(expected.animations.at(j), actual.animations.at(j));
    }
  }
}
