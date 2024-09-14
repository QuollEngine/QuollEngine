#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetFileHeader.h"
#include "quoll/asset/InputBinaryStream.h"
#include "quoll/renderer/MaterialAsset.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include <random>

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
    asset.data.metallicFactor = 1.0f;
    asset.data.roughnessFactor = 2.5f;
    asset.data.metallicRoughnessTextureCoord = 3;
    asset.data.normalScale = 0.6f;
    asset.data.normalTextureCoord = 4;
    asset.data.occlusionStrength = 0.4f;
    asset.data.occlusionTextureCoord = 5;
    asset.data.emissiveFactor = glm::vec3(0.5f, 0.6f, 2.5f);
    asset.data.emissiveTextureCoord = 6;

    if (createTextures) {
      asset.data.baseColorTexture = createAsset<quoll::TextureAsset>();
      asset.data.metallicRoughnessTexture = createAsset<quoll::TextureAsset>();
      asset.data.normalTexture = createAsset<quoll::TextureAsset>();
      asset.data.occlusionTexture = createAsset<quoll::TextureAsset>();
      asset.data.emissiveTexture = createAsset<quoll::TextureAsset>();
    }

    return asset;
  }

  void SetUp() override {
    AssetCacheTestBase::SetUp();

    textureUuid = quoll::Uuid::generate();
    cache.createFromSource<quoll::TextureAsset>(FixturesPath / "1x1-2d.ktx",
                                                textureUuid);
  }

  quoll::Uuid textureUuid;
};

TEST_F(AssetCacheMaterialTest, CreatesMetaFileFromAsset) {
  auto asset = createMaterialAsset(true);
  auto filePath = cache.createFromData(asset);
  auto meta = cache.getAssetMeta(asset.uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Material);
  EXPECT_EQ(meta.name, "material1");
}

TEST_F(AssetCacheMaterialTest, CreatesMaterialWithTexturesFromAsset) {
  auto asset = createMaterialAsset(true);
  auto filePath = cache.createFromData(asset);

  ASSERT_TRUE(filePath);
  EXPECT_FALSE(filePath.hasWarnings());

  {
    quoll::InputBinaryStream file(filePath);
    EXPECT_TRUE(file.good());

    quoll::AssetFileHeader header;
    file.read(header);

    // Base color
    quoll::Uuid baseTextureUuid;
    file.read(baseTextureUuid);
    i8 baseTextureCoord = -1;
    file.read(baseTextureCoord);
    glm::vec4 baseColorFactor;
    file.read(baseColorFactor);

    // Metallic roughness
    quoll::Uuid metallicRoughnessUuid;
    file.read(metallicRoughnessUuid);
    i8 metallicRoughnessTextureCoord = -1;
    file.read(metallicRoughnessTextureCoord);
    f32 metallicFactor = 0.0f;
    file.read(metallicFactor);
    f32 roughnessFactor = 0.0f;
    file.read(roughnessFactor);

    // Normal
    quoll::Uuid normalTextureUuid;
    file.read(normalTextureUuid);
    i8 normalTextureCoord = -1;
    file.read(normalTextureCoord);
    f32 normalScale = 0.0f;
    file.read(normalScale);

    // Occlusion
    quoll::Uuid occlusionTextureUuid;
    file.read(occlusionTextureUuid);
    i8 occlusionTextureCoord = -1;
    file.read(occlusionTextureCoord);
    f32 occlusionStrength = 0.0f;
    file.read(occlusionStrength);

    // Emissive
    quoll::Uuid emissiveTextureUuid;
    file.read(emissiveTextureUuid);
    i8 emissiveTextureCoord = -1;
    file.read(emissiveTextureCoord);
    glm::vec3 emissiveFactor;
    file.read(emissiveFactor);

    EXPECT_EQ(baseTextureUuid, asset.data.baseColorTexture.meta().uuid);
    EXPECT_EQ(baseTextureCoord, 2);
    EXPECT_EQ(baseColorFactor, glm::vec4(2.5f, 0.2f, 0.5f, 5.2f));
    EXPECT_EQ(metallicRoughnessUuid,
              asset.data.metallicRoughnessTexture.meta().uuid);
    EXPECT_EQ(metallicRoughnessTextureCoord, 3);
    EXPECT_EQ(metallicFactor, 1.0f);
    EXPECT_EQ(roughnessFactor, 2.5f);
    EXPECT_EQ(normalTextureUuid, asset.data.normalTexture.meta().uuid);
    EXPECT_EQ(normalTextureCoord, 4);
    EXPECT_EQ(normalScale, 0.6f);
    EXPECT_EQ(occlusionTextureUuid, asset.data.occlusionTexture.meta().uuid);
    EXPECT_EQ(occlusionTextureCoord, 5);
    EXPECT_EQ(occlusionStrength, 0.4f);
    EXPECT_EQ(emissiveTextureUuid, asset.data.emissiveTexture.meta().uuid);
    EXPECT_EQ(emissiveTextureCoord, 6);
    EXPECT_EQ(emissiveFactor, glm::vec3(0.5f, 0.6f, 2.5f));
  }

  EXPECT_TRUE(
      std::filesystem::exists(filePath.data().replace_extension("assetmeta")));
}

TEST_F(AssetCacheMaterialTest,
       CreatesMaterialWithoutTexturesFromAssetIfReferencedTexturesAreInvalid) {
  auto asset = createMaterialAsset(false);

  auto filePath = cache.createFromData(asset);

  {
    quoll::InputBinaryStream file(filePath);
    EXPECT_TRUE(file.good());

    quoll::AssetFileHeader header;
    file.read(header);

    // Base color
    quoll::String baseTexturePath;
    file.read(baseTexturePath);
    i8 baseTextureCoord = -1;
    file.read(baseTextureCoord);
    glm::vec4 baseColorFactor;
    file.read(baseColorFactor);

    // Metallic roughness
    quoll::String metallicRoughnessTexturePath;
    file.read(metallicRoughnessTexturePath);
    i8 metallicRoughnessTextureCoord = -1;
    file.read(metallicRoughnessTextureCoord);
    f32 metallicFactor = 0.0f;
    file.read(metallicFactor);
    f32 roughnessFactor = 0.0f;
    file.read(roughnessFactor);

    // Normal
    quoll::String normalTexturePath;
    file.read(normalTexturePath);
    i8 normalTextureCoord = -1;
    file.read(normalTextureCoord);
    f32 normalScale = 0.0f;
    file.read(normalScale);

    // Occlusion
    quoll::String occlusionTexturePath;
    file.read(occlusionTexturePath);
    i8 occlusionTextureCoord = -1;
    file.read(occlusionTextureCoord);
    f32 occlusionStrength = 0.0f;
    file.read(occlusionStrength);

    // Emissive
    quoll::String emissiveTexturePath;
    file.read(emissiveTexturePath);
    i8 emissiveTextureCoord = -1;
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

  auto assetFile = cache.createFromData(asset);
  EXPECT_TRUE(assetFile);
  EXPECT_FALSE(assetFile.hasWarnings());

  auto res = requestAndWait<quoll::MaterialAsset>(asset.uuid);

  EXPECT_TRUE(res);
  EXPECT_FALSE(res.hasWarnings());

  auto material = res.data();
  EXPECT_NE(material.handle(), quoll::AssetHandle<quoll::MaterialAsset>());

  EXPECT_EQ(material.meta().name, asset.name);
  EXPECT_TRUE(material.meta().uuid.isValid());
  EXPECT_EQ(material.meta().type, quoll::AssetType::Material);

  EXPECT_EQ(material->baseColorTexture, asset.data.baseColorTexture);
  EXPECT_EQ(material->baseColorTextureCoord, asset.data.baseColorTextureCoord);
  EXPECT_EQ(material->baseColorFactor, asset.data.baseColorFactor);

  EXPECT_EQ(material->metallicRoughnessTexture,
            asset.data.metallicRoughnessTexture);
  EXPECT_EQ(material->metallicRoughnessTextureCoord,
            asset.data.metallicRoughnessTextureCoord);
  EXPECT_EQ(material->metallicFactor, asset.data.metallicFactor);
  EXPECT_EQ(material->roughnessFactor, asset.data.roughnessFactor);

  EXPECT_EQ(material->normalTexture, asset.data.normalTexture);
  EXPECT_EQ(material->normalTextureCoord, asset.data.normalTextureCoord);
  EXPECT_EQ(material->normalScale, asset.data.normalScale);

  EXPECT_EQ(material->occlusionTexture, asset.data.occlusionTexture);
  EXPECT_EQ(material->occlusionTextureCoord, asset.data.occlusionTextureCoord);
  EXPECT_EQ(material->occlusionStrength, asset.data.occlusionStrength);

  EXPECT_EQ(material->emissiveTexture, asset.data.emissiveTexture);
  EXPECT_EQ(material->emissiveTextureCoord, asset.data.emissiveTextureCoord);
  EXPECT_EQ(material->emissiveFactor, asset.data.emissiveFactor);
}

TEST_F(AssetCacheMaterialTest, LoadsMaterialWithoutTexturesFromFile) {
  auto asset = createMaterialAsset(false);

  auto assetFile = cache.createFromData(asset);
  EXPECT_TRUE(assetFile);
  EXPECT_FALSE(assetFile.hasWarnings());

  auto res = requestAndWait<quoll::MaterialAsset>(asset.uuid);

  EXPECT_TRUE(res);
  EXPECT_FALSE(res.hasWarnings());
  EXPECT_EQ(res.warnings().size(), 0);

  auto material = res.data();

  EXPECT_NE(material.handle(), quoll::AssetHandle<quoll::MaterialAsset>());

  EXPECT_TRUE(material.meta().uuid.isValid());
  EXPECT_EQ(material.meta().type, quoll::AssetType::Material);

  EXPECT_EQ(material->baseColorTexture, asset.data.baseColorTexture);
  EXPECT_EQ(material->baseColorTextureCoord, asset.data.baseColorTextureCoord);
  EXPECT_EQ(material->baseColorFactor, asset.data.baseColorFactor);

  EXPECT_EQ(material->metallicRoughnessTexture,
            asset.data.metallicRoughnessTexture);
  EXPECT_EQ(material->metallicRoughnessTextureCoord,
            asset.data.metallicRoughnessTextureCoord);
  EXPECT_EQ(material->metallicFactor, asset.data.metallicFactor);
  EXPECT_EQ(material->roughnessFactor, asset.data.roughnessFactor);

  EXPECT_EQ(material->normalTexture, asset.data.normalTexture);
  EXPECT_EQ(material->normalTextureCoord, asset.data.normalTextureCoord);
  EXPECT_EQ(material->normalScale, asset.data.normalScale);

  EXPECT_EQ(material->occlusionTexture, asset.data.occlusionTexture);
  EXPECT_EQ(material->occlusionTextureCoord, asset.data.occlusionTextureCoord);
  EXPECT_EQ(material->occlusionStrength, asset.data.occlusionStrength);

  EXPECT_EQ(material->emissiveTexture, asset.data.emissiveTexture);
  EXPECT_EQ(material->emissiveTextureCoord, asset.data.emissiveTextureCoord);
  EXPECT_EQ(material->emissiveFactor, asset.data.emissiveFactor);
}

TEST_F(AssetCacheMaterialTest, LoadsMaterialWithTextures) {
  auto materialUuid = quoll::Uuid::generate();

  {
    auto texture = requestAndWait<quoll::TextureAsset>(textureUuid).data();
    quoll::AssetData<quoll::MaterialAsset> material{};
    material.uuid = materialUuid;
    material.data.baseColorTexture = texture;
    auto path = cache.createFromData(material);
  }

  cache.getRegistry().clear<quoll::TextureAsset>();

  auto res = requestAndWait<quoll::MaterialAsset>(materialUuid);
  ASSERT_TRUE(res);

  auto newMaterial = res.data();
  EXPECT_TRUE(newMaterial->baseColorTexture);

  auto &newTexture = newMaterial->baseColorTexture;
  EXPECT_EQ(newTexture.meta().uuid, textureUuid);
}
