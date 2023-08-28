#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"
#include "liquid-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheMaterialTest : public AssetCacheTestBase {
public:
  liquid::AssetData<liquid::MaterialAsset>
  createMaterialAsset(bool createTextures) {
    liquid::AssetData<liquid::MaterialAsset> asset{};
    asset.name = "material1";
    asset.uuid = liquid::Uuid("material1.uuid");
    asset.type = liquid::AssetType::Material;
    asset.data.baseColorFactor = glm::vec4(2.5f, 0.2f, 0.5f, 5.2f);
    asset.data.baseColorTextureCoord = 2;

    if (createTextures) {
      liquid::AssetData<liquid::TextureAsset> texture;
      texture.uuid = liquid::Uuid("base");

      asset.data.baseColorTexture =
          cache.getRegistry().getTextures().addAsset(texture);
    }

    asset.data.metallicFactor = 1.0f;
    asset.data.roughnessFactor = 2.5f;
    asset.data.metallicRoughnessTextureCoord = 3;
    if (createTextures) {
      liquid::AssetData<liquid::TextureAsset> texture;
      texture.uuid = liquid::Uuid("mr");

      asset.data.metallicRoughnessTexture =
          cache.getRegistry().getTextures().addAsset(texture);
    }

    asset.data.normalScale = 0.6f;
    asset.data.normalTextureCoord = 4;
    if (createTextures) {
      liquid::AssetData<liquid::TextureAsset> texture;
      texture.uuid = liquid::Uuid("normal");

      asset.data.normalTexture =
          cache.getRegistry().getTextures().addAsset(texture);
    }

    asset.data.occlusionStrength = 0.4f;
    asset.data.occlusionTextureCoord = 5;
    if (createTextures) {
      liquid::AssetData<liquid::TextureAsset> texture;
      texture.uuid = liquid::Uuid("occlusion");

      asset.data.occlusionTexture =
          cache.getRegistry().getTextures().addAsset(texture);
    }

    asset.data.emissiveFactor = glm::vec3(0.5f, 0.6f, 2.5f);
    asset.data.emissiveTextureCoord = 6;
    if (createTextures) {
      liquid::AssetData<liquid::TextureAsset> texture;
      texture.uuid = liquid::Uuid("emissive");

      asset.data.emissiveTexture =
          cache.getRegistry().getTextures().addAsset(texture);
    }

    return asset;
  }

  void SetUp() override {
    AssetCacheTestBase::SetUp();

    textureUuid = liquid::Uuid::generate();
    cache.createTextureFromSource(FixturesPath / "1x1-2d.ktx", textureUuid);
  }

  liquid::Uuid textureUuid;
};

TEST_F(AssetCacheMaterialTest, CreatesMaterialWithTexturesFromAsset) {
  auto asset = createMaterialAsset(true);
  auto filePath = cache.createMaterialFromAsset(asset);

  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  {
    liquid::InputBinaryStream file(filePath.getData());
    EXPECT_TRUE(file.good());

    liquid::AssetFileHeader header;
    file.read(header);

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

    EXPECT_EQ(baseTexturePath, "base");
    EXPECT_EQ(baseTextureCoord, 2);
    EXPECT_EQ(baseColorFactor, glm::vec4(2.5f, 0.2f, 0.5f, 5.2f));
    EXPECT_EQ(metallicRoughnessTexturePath, "mr");
    EXPECT_EQ(metallicRoughnessTextureCoord, 3);
    EXPECT_EQ(metallicFactor, 1.0f);
    EXPECT_EQ(roughnessFactor, 2.5f);
    EXPECT_EQ(normalTexturePath, "normal");
    EXPECT_EQ(normalTextureCoord, 4);
    EXPECT_EQ(normalScale, 0.6f);
    EXPECT_EQ(occlusionTexturePath, "occlusion");
    EXPECT_EQ(occlusionTextureCoord, 5);
    EXPECT_EQ(occlusionStrength, 0.4f);
    EXPECT_EQ(emissiveTexturePath, "emissive");
    EXPECT_EQ(emissiveTextureCoord, 6);
    EXPECT_EQ(emissiveFactor, glm::vec3(0.5f, 0.6f, 2.5f));
  }

  EXPECT_FALSE(std::filesystem::exists(
      filePath.getData().replace_extension("assetmeta")));
}

TEST_F(AssetCacheMaterialTest,
       CreatesMaterialWithoutTexturesFromAssetIfReferencedTexturesAreInvalid) {
  auto asset = createMaterialAsset(false);

  auto filePath = cache.createMaterialFromAsset(asset);

  {
    liquid::InputBinaryStream file(filePath.getData());
    EXPECT_TRUE(file.good());

    liquid::AssetFileHeader header;
    file.read(header);

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

    EXPECT_EQ(header.magic, header.MagicConstant);
    EXPECT_EQ(header.type, liquid::AssetType::Material);
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
}

TEST_F(AssetCacheMaterialTest, LoadsMaterialWithTexturesFromFile) {
  auto asset = createMaterialAsset(true);

  auto assetFile = cache.createMaterialFromAsset(asset);
  EXPECT_FALSE(assetFile.hasError());
  EXPECT_FALSE(assetFile.hasWarnings());

  auto res = cache.loadMaterial(asset.uuid);

  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  auto handle = res.getData();

  EXPECT_NE(handle, liquid::MaterialAssetHandle::Null);

  auto &material = cache.getRegistry().getMaterials().getAsset(handle);
  EXPECT_EQ(material.name, asset.name);
  EXPECT_TRUE(material.uuid.isValid());
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

TEST_F(AssetCacheMaterialTest, LoadsMaterialWithoutTexturesFromFile) {
  auto asset = createMaterialAsset(false);

  auto assetFile = cache.createMaterialFromAsset(asset);
  EXPECT_FALSE(assetFile.hasError());
  EXPECT_FALSE(assetFile.hasWarnings());

  auto res = cache.loadMaterial(asset.uuid);

  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  EXPECT_EQ(res.getWarnings().size(), 0);

  auto handle = res.getData();

  EXPECT_NE(handle, liquid::MaterialAssetHandle::Null);

  auto &material = cache.getRegistry().getMaterials().getAsset(handle);
  EXPECT_TRUE(material.uuid.isValid());
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

TEST_F(AssetCacheMaterialTest, LoadsTexturesWithMaterials) {
  auto texture = cache.loadTexture(textureUuid);
  liquid::AssetData<liquid::MaterialAsset> material{};
  material.uuid = liquid::Uuid::generate();
  material.data.baseColorTexture = texture.getData();
  auto path = cache.createMaterialFromAsset(material);

  cache.getRegistry().getTextures().deleteAsset(texture.getData());
  EXPECT_FALSE(cache.getRegistry().getTextures().hasAsset(texture.getData()));

  auto handle = cache.loadMaterial(material.uuid);

  auto &newMaterial =
      cache.getRegistry().getMaterials().getAsset(handle.getData());

  EXPECT_NE(newMaterial.data.baseColorTexture,
            liquid::TextureAssetHandle::Null);

  auto &newTexture = cache.getRegistry().getTextures().getAsset(
      newMaterial.data.baseColorTexture);
  EXPECT_EQ(newTexture.uuid, textureUuid);
}
