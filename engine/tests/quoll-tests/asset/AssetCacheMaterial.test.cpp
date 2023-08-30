#include "quoll/core/Base.h"
#include <random>

#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/AssetFileHeader.h"
#include "quoll/asset/InputBinaryStream.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheMaterialTest : public AssetCacheTestBase {
public:
  quoll::AssetData<quoll::MaterialAsset>
  createMaterialAsset(bool createTextures) {
    quoll::AssetData<quoll::MaterialAsset> asset{};
    asset.name = "material1";
    asset.uuid = quoll::Uuid("material1.uuid");
    asset.type = quoll::AssetType::Material;
    asset.data.baseColorFactor = glm::vec4(2.5f, 0.2f, 0.5f, 5.2f);
    asset.data.baseColorTextureCoord = 2;

    if (createTextures) {
      quoll::AssetData<quoll::TextureAsset> texture;
      texture.uuid = quoll::Uuid("base");

      asset.data.baseColorTexture =
          cache.getRegistry().getTextures().addAsset(texture);
    }

    asset.data.metallicFactor = 1.0f;
    asset.data.roughnessFactor = 2.5f;
    asset.data.metallicRoughnessTextureCoord = 3;
    if (createTextures) {
      quoll::AssetData<quoll::TextureAsset> texture;
      texture.uuid = quoll::Uuid("mr");

      asset.data.metallicRoughnessTexture =
          cache.getRegistry().getTextures().addAsset(texture);
    }

    asset.data.normalScale = 0.6f;
    asset.data.normalTextureCoord = 4;
    if (createTextures) {
      quoll::AssetData<quoll::TextureAsset> texture;
      texture.uuid = quoll::Uuid("normal");

      asset.data.normalTexture =
          cache.getRegistry().getTextures().addAsset(texture);
    }

    asset.data.occlusionStrength = 0.4f;
    asset.data.occlusionTextureCoord = 5;
    if (createTextures) {
      quoll::AssetData<quoll::TextureAsset> texture;
      texture.uuid = quoll::Uuid("occlusion");

      asset.data.occlusionTexture =
          cache.getRegistry().getTextures().addAsset(texture);
    }

    asset.data.emissiveFactor = glm::vec3(0.5f, 0.6f, 2.5f);
    asset.data.emissiveTextureCoord = 6;
    if (createTextures) {
      quoll::AssetData<quoll::TextureAsset> texture;
      texture.uuid = quoll::Uuid("emissive");

      asset.data.emissiveTexture =
          cache.getRegistry().getTextures().addAsset(texture);
    }

    return asset;
  }

  void SetUp() override {
    AssetCacheTestBase::SetUp();

    textureUuid = quoll::Uuid::generate();
    cache.createTextureFromSource(FixturesPath / "1x1-2d.ktx", textureUuid);
  }

  quoll::Uuid textureUuid;
};

TEST_F(AssetCacheMaterialTest, CreatesMaterialWithTexturesFromAsset) {
  auto asset = createMaterialAsset(true);
  auto filePath = cache.createMaterialFromAsset(asset);

  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  {
    quoll::InputBinaryStream file(filePath.getData());
    EXPECT_TRUE(file.good());

    quoll::AssetFileHeader header;
    file.read(header);

    // Base color
    quoll::String baseTexturePath;
    file.read(baseTexturePath);
    int8_t baseTextureCoord = -1;
    file.read(baseTextureCoord);
    glm::vec4 baseColorFactor;
    file.read(baseColorFactor);

    // Metallic roughness
    quoll::String metallicRoughnessTexturePath;
    file.read(metallicRoughnessTexturePath);
    int8_t metallicRoughnessTextureCoord = -1;
    file.read(metallicRoughnessTextureCoord);
    float metallicFactor = 0.0f;
    file.read(metallicFactor);
    float roughnessFactor = 0.0f;
    file.read(roughnessFactor);

    // Normal
    quoll::String normalTexturePath;
    file.read(normalTexturePath);
    int8_t normalTextureCoord = -1;
    file.read(normalTextureCoord);
    float normalScale = 0.0f;
    file.read(normalScale);

    // Occlusion
    quoll::String occlusionTexturePath;
    file.read(occlusionTexturePath);
    int8_t occlusionTextureCoord = -1;
    file.read(occlusionTextureCoord);
    float occlusionStrength = 0.0f;
    file.read(occlusionStrength);

    // Emissive
    quoll::String emissiveTexturePath;
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
    quoll::InputBinaryStream file(filePath.getData());
    EXPECT_TRUE(file.good());

    quoll::AssetFileHeader header;
    file.read(header);

    // Base color
    quoll::String baseTexturePath;
    file.read(baseTexturePath);
    int8_t baseTextureCoord = -1;
    file.read(baseTextureCoord);
    glm::vec4 baseColorFactor;
    file.read(baseColorFactor);

    // Metallic roughness
    quoll::String metallicRoughnessTexturePath;
    file.read(metallicRoughnessTexturePath);
    int8_t metallicRoughnessTextureCoord = -1;
    file.read(metallicRoughnessTextureCoord);
    float metallicFactor = 0.0f;
    file.read(metallicFactor);
    float roughnessFactor = 0.0f;
    file.read(roughnessFactor);

    // Normal
    quoll::String normalTexturePath;
    file.read(normalTexturePath);
    int8_t normalTextureCoord = -1;
    file.read(normalTextureCoord);
    float normalScale = 0.0f;
    file.read(normalScale);

    // Occlusion
    quoll::String occlusionTexturePath;
    file.read(occlusionTexturePath);
    int8_t occlusionTextureCoord = -1;
    file.read(occlusionTextureCoord);
    float occlusionStrength = 0.0f;
    file.read(occlusionStrength);

    // Emissive
    quoll::String emissiveTexturePath;
    file.read(emissiveTexturePath);
    int8_t emissiveTextureCoord = -1;
    file.read(emissiveTextureCoord);
    glm::vec3 emissiveFactor;
    file.read(emissiveFactor);

    EXPECT_EQ(header.magic, header.MagicConstant);
    EXPECT_EQ(header.type, quoll::AssetType::Material);
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

  EXPECT_NE(handle, quoll::MaterialAssetHandle::Null);

  auto &material = cache.getRegistry().getMaterials().getAsset(handle);
  EXPECT_EQ(material.name, asset.name);
  EXPECT_TRUE(material.uuid.isValid());
  EXPECT_EQ(material.type, quoll::AssetType::Material);

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

  EXPECT_NE(handle, quoll::MaterialAssetHandle::Null);

  auto &material = cache.getRegistry().getMaterials().getAsset(handle);
  EXPECT_TRUE(material.uuid.isValid());
  EXPECT_EQ(material.type, quoll::AssetType::Material);

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
  quoll::AssetData<quoll::MaterialAsset> material{};
  material.uuid = quoll::Uuid::generate();
  material.data.baseColorTexture = texture.getData();
  auto path = cache.createMaterialFromAsset(material);

  cache.getRegistry().getTextures().deleteAsset(texture.getData());
  EXPECT_FALSE(cache.getRegistry().getTextures().hasAsset(texture.getData()));

  auto handle = cache.loadMaterial(material.uuid);

  auto &newMaterial =
      cache.getRegistry().getMaterials().getAsset(handle.getData());

  EXPECT_NE(newMaterial.data.baseColorTexture, quoll::TextureAssetHandle::Null);

  auto &newTexture = cache.getRegistry().getTextures().getAsset(
      newMaterial.data.baseColorTexture);
  EXPECT_EQ(newTexture.uuid, textureUuid);
}
