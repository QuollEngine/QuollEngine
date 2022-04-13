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
