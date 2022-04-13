#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/asset/AssetFileHeader.h"

#include <gtest/gtest.h>

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

  std::ifstream file(assetFile, std::ios::binary | std::ios::in);
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;

  liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), magic.size());

  file.read(reinterpret_cast<char *>(&header.version), sizeof(header.version));
  file.read(reinterpret_cast<char *>(&header.type), sizeof(header.type));

  // Base color
  uint32_t baseTextureLength = 0;
  file.read(reinterpret_cast<char *>(&baseTextureLength), sizeof(uint32_t));
  liquid::String baseTexturePath(baseTextureLength, '$');
  file.read(baseTexturePath.data(), baseTextureLength);
  int8_t baseTextureCoord = -1;
  file.read(reinterpret_cast<char *>(&baseTextureCoord), sizeof(int8_t));
  glm::vec4 baseColorFactor;
  file.read(reinterpret_cast<char *>(&baseColorFactor), sizeof(float) * 4);

  // Metallic roughness
  uint32_t metallicRoughnessTextureLength = 0;
  file.read(reinterpret_cast<char *>(&metallicRoughnessTextureLength),
            sizeof(uint32_t));
  liquid::String metallicRoughnessTexturePath(metallicRoughnessTextureLength,
                                              '$');
  file.read(metallicRoughnessTexturePath.data(),
            metallicRoughnessTextureLength);
  int8_t metallicRoughnessTextureCoord = -1;
  file.read(reinterpret_cast<char *>(&metallicRoughnessTextureCoord),
            sizeof(int8_t));
  float metallicFactor = 0.0f;
  file.read(reinterpret_cast<char *>(&metallicFactor), sizeof(float));
  float roughnessFactor = 0.0f;
  file.read(reinterpret_cast<char *>(&roughnessFactor), sizeof(float));

  // Normal
  uint32_t normalTextureLength = 0;
  file.read(reinterpret_cast<char *>(&normalTextureLength), sizeof(uint32_t));
  liquid::String normalTexturePath(normalTextureLength, '$');
  file.read(normalTexturePath.data(), normalTextureLength);
  int8_t normalTextureCoord = -1;
  file.read(reinterpret_cast<char *>(&normalTextureCoord), sizeof(int8_t));
  float normalScale = 0.0f;
  file.read(reinterpret_cast<char *>(&normalScale), sizeof(float));

  // Occlusion
  uint32_t occlusionTextureLength = 0;
  file.read(reinterpret_cast<char *>(&occlusionTextureLength),
            sizeof(uint32_t));
  liquid::String occlusionTexturePath(occlusionTextureLength, '$');
  file.read(occlusionTexturePath.data(), occlusionTextureLength);
  int8_t occlusionTextureCoord = -1;
  file.read(reinterpret_cast<char *>(&occlusionTextureCoord), sizeof(int8_t));
  float occlusionStrength = 0.0f;
  file.read(reinterpret_cast<char *>(&occlusionStrength), sizeof(float));

  // Emissive
  uint32_t emissiveTextureLength = 0;
  file.read(reinterpret_cast<char *>(&emissiveTextureLength), sizeof(uint32_t));
  liquid::String emissiveTexturePath(emissiveTextureLength, '$');
  file.read(emissiveTexturePath.data(), emissiveTextureLength);
  int8_t emissiveTextureCoord = -1;
  file.read(reinterpret_cast<char *>(&emissiveTextureCoord), sizeof(int8_t));
  glm::vec3 emissiveFactor;
  file.read(reinterpret_cast<char *>(&emissiveFactor), sizeof(float) * 3);

  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.type, liquid::AssetType::Material);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(baseTextureLength, 18);
  EXPECT_EQ(baseTexturePath, "textures/test.ktx2");
  EXPECT_EQ(baseTextureCoord, 2);
  EXPECT_EQ(baseColorFactor, glm::vec4(2.5f, 0.2f, 0.5f, 5.2f));
  EXPECT_EQ(metallicRoughnessTextureLength, 16);
  EXPECT_EQ(metallicRoughnessTexturePath, "textures/mr.ktx2");
  EXPECT_EQ(metallicRoughnessTextureCoord, 3);
  EXPECT_EQ(metallicFactor, 1.0f);
  EXPECT_EQ(roughnessFactor, 2.5f);
  EXPECT_EQ(normalTextureLength, 20);
  EXPECT_EQ(normalTexturePath, "textures/normal.ktx2");
  EXPECT_EQ(normalTextureCoord, 4);
  EXPECT_EQ(normalScale, 0.6f);
  EXPECT_EQ(occlusionTextureLength, 23);
  EXPECT_EQ(occlusionTexturePath, "textures/occlusion.ktx2");
  EXPECT_EQ(occlusionTextureCoord, 5);
  EXPECT_EQ(occlusionStrength, 0.4f);
  EXPECT_EQ(emissiveTextureLength, 22);
  EXPECT_EQ(emissiveTexturePath, "textures/emissive.ktx2");
  EXPECT_EQ(emissiveTextureCoord, 6);
  EXPECT_EQ(emissiveFactor, glm::vec3(0.5f, 0.6f, 2.5f));

  file.close();
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

  std::ifstream file(assetFile, std::ios::binary | std::ios::in);
  EXPECT_TRUE(file.good());

  liquid::AssetFileHeader header;
  liquid::String magic(liquid::ASSET_FILE_MAGIC_LENGTH, '$');
  file.read(magic.data(), magic.length());
  file.read(reinterpret_cast<char *>(&header.version), sizeof(header.version));
  file.read(reinterpret_cast<char *>(&header.type), sizeof(header.type));

  // Base color
  uint32_t baseTextureLength = 0;
  file.read(reinterpret_cast<char *>(&baseTextureLength), sizeof(uint32_t));
  liquid::String baseTexturePath(baseTextureLength, '$');
  file.read(baseTexturePath.data(), baseTextureLength);
  int8_t baseTextureCoord = -1;
  file.read(reinterpret_cast<char *>(&baseTextureCoord), sizeof(int8_t));
  glm::vec4 baseColorFactor;
  file.read(reinterpret_cast<char *>(&baseColorFactor), sizeof(float) * 4);

  // Metallic roughness
  uint32_t metallicRoughnessTextureLength = 0;
  file.read(reinterpret_cast<char *>(&metallicRoughnessTextureLength),
            sizeof(uint32_t));
  liquid::String metallicRoughnessTexturePath(metallicRoughnessTextureLength,
                                              '$');
  file.read(metallicRoughnessTexturePath.data(),
            metallicRoughnessTextureLength);
  int8_t metallicRoughnessTextureCoord = -1;
  file.read(reinterpret_cast<char *>(&metallicRoughnessTextureCoord),
            sizeof(int8_t));
  float metallicFactor = 0.0f;
  file.read(reinterpret_cast<char *>(&metallicFactor), sizeof(float));
  float roughnessFactor = 0.0f;
  file.read(reinterpret_cast<char *>(&roughnessFactor), sizeof(float));

  // Normal
  uint32_t normalTextureLength = 0;
  file.read(reinterpret_cast<char *>(&normalTextureLength), sizeof(uint32_t));
  liquid::String normalTexturePath(normalTextureLength, '$');
  file.read(normalTexturePath.data(), normalTextureLength);
  int8_t normalTextureCoord = -1;
  file.read(reinterpret_cast<char *>(&normalTextureCoord), sizeof(int8_t));
  float normalScale = 0.0f;
  file.read(reinterpret_cast<char *>(&normalScale), sizeof(float));

  // Occlusion
  uint32_t occlusionTextureLength = 0;
  file.read(reinterpret_cast<char *>(&occlusionTextureLength),
            sizeof(uint32_t));
  liquid::String occlusionTexturePath(occlusionTextureLength, '$');
  file.read(occlusionTexturePath.data(), occlusionTextureLength);
  int8_t occlusionTextureCoord = -1;
  file.read(reinterpret_cast<char *>(&occlusionTextureCoord), sizeof(int8_t));
  float occlusionStrength = 0.0f;
  file.read(reinterpret_cast<char *>(&occlusionStrength), sizeof(float));

  // Emissive
  uint32_t emissiveTextureLength = 0;
  file.read(reinterpret_cast<char *>(&emissiveTextureLength), sizeof(uint32_t));
  liquid::String emissiveTexturePath(emissiveTextureLength, '$');
  file.read(emissiveTexturePath.data(), emissiveTextureLength);
  int8_t emissiveTextureCoord = -1;
  file.read(reinterpret_cast<char *>(&emissiveTextureCoord), sizeof(int8_t));
  glm::vec3 emissiveFactor;
  file.read(reinterpret_cast<char *>(&emissiveFactor), sizeof(float) * 3);

  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.type, liquid::AssetType::Material);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(baseTextureLength, 0);
  EXPECT_EQ(baseTexturePath, "");
  EXPECT_EQ(baseTextureCoord, 2);
  EXPECT_EQ(baseColorFactor, glm::vec4(2.5f, 0.2f, 0.5f, 5.2f));
  EXPECT_EQ(metallicRoughnessTextureLength, 0);
  EXPECT_EQ(metallicRoughnessTexturePath, "");
  EXPECT_EQ(metallicRoughnessTextureCoord, 3);
  EXPECT_EQ(metallicFactor, 1.0f);
  EXPECT_EQ(roughnessFactor, 2.5f);
  EXPECT_EQ(normalTextureLength, 0);
  EXPECT_EQ(normalTexturePath, "");
  EXPECT_EQ(normalTextureCoord, 4);
  EXPECT_EQ(normalScale, 0.6f);
  EXPECT_EQ(occlusionTextureLength, 0);
  EXPECT_EQ(occlusionTexturePath, "");
  EXPECT_EQ(occlusionTextureCoord, 5);
  EXPECT_EQ(occlusionStrength, 0.4f);
  EXPECT_EQ(emissiveTextureLength, 0);
  EXPECT_EQ(emissiveTexturePath, "");
  EXPECT_EQ(emissiveTextureCoord, 6);
  EXPECT_EQ(emissiveFactor, glm::vec3(0.5f, 0.6f, 2.5f));

  file.close();
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
}
