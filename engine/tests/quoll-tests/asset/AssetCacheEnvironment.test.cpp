#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/AssetFileHeader.h"
#include "quoll/asset/InputBinaryStream.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheEnvironmentTest : public AssetCacheTestBase {
public:
  void SetUp() override {
    AssetCacheTestBase::SetUp();

    irradianceUuid = quoll::Uuid::generate();
    specularUuid = quoll::Uuid::generate();

    cache.createFromSource<quoll::TextureAsset>(
        FixturesPath / "1x1-cubemap.ktx", irradianceUuid);
    cache.createFromSource<quoll::TextureAsset>(
        FixturesPath / "1x1-cubemap.ktx", specularUuid);
  }

  quoll::Uuid irradianceUuid;
  quoll::Uuid specularUuid;
};

TEST_F(AssetCacheEnvironmentTest, CreatesMetaFileFromAsset) {
  auto irradianceMap = cache.request<quoll::TextureAsset>(irradianceUuid);
  auto specularMap = cache.request<quoll::TextureAsset>(specularUuid);

  ASSERT_TRUE(irradianceMap);
  ASSERT_TRUE(specularMap);

  quoll::AssetData<quoll::EnvironmentAsset> asset{};
  asset.name = "env-0";
  asset.uuid = quoll::Uuid::generate();
  asset.data.irradianceMap = irradianceMap.data();
  asset.data.specularMap = irradianceMap.data();

  auto filePath = cache.createFromData(asset);
  auto meta = cache.getAssetMeta(asset.uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::Environment);
  EXPECT_EQ(meta.name, "env-0");
}

TEST_F(AssetCacheEnvironmentTest, CreatesEnvironmentFileFromEnvironmentAsset) {
  auto irradianceMap = cache.request<quoll::TextureAsset>(irradianceUuid);
  auto specularMap = cache.request<quoll::TextureAsset>(specularUuid);

  ASSERT_TRUE(irradianceMap);
  ASSERT_TRUE(specularMap);

  quoll::AssetData<quoll::EnvironmentAsset> asset{};
  asset.name = "env-0";
  asset.uuid = quoll::Uuid::generate();
  asset.data.irradianceMap = irradianceMap.data();
  asset.data.specularMap = specularMap.data();

  auto filePath = cache.createFromData(asset);
  EXPECT_TRUE(filePath);
  EXPECT_FALSE(filePath.hasWarnings());

  quoll::InputBinaryStream file(filePath);
  EXPECT_TRUE(file.good());
  quoll::AssetFileHeader header;
  file.read(header);
  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.type, quoll::AssetType::Environment);

  quoll::Uuid readIrradianceUuid;
  file.read(readIrradianceUuid);

  quoll::Uuid readSpecularUuid;
  file.read(readSpecularUuid);

  EXPECT_EQ(readIrradianceUuid, irradianceUuid);
  EXPECT_EQ(readSpecularUuid, specularUuid);
}

TEST_F(AssetCacheEnvironmentTest,
       DoesNotLoadEnvironmentAssetIfEnvironmentTexturesCouldNotBeLoaded) {
  auto irradianceMap = cache.request<quoll::TextureAsset>(irradianceUuid);
  auto specularMap = cache.request<quoll::TextureAsset>(specularUuid);

  ASSERT_TRUE(irradianceMap);
  ASSERT_TRUE(specularMap);

  quoll::AssetData<quoll::EnvironmentAsset> asset{};
  asset.uuid = quoll::Uuid::generate();
  asset.data.irradianceMap = irradianceMap.data();
  asset.data.specularMap = specularMap.data();
  auto createRes = cache.createFromData(asset);

  cache.getRegistry().remove(irradianceMap.data().handle());
  cache.getRegistry().remove(specularMap.data().handle());

  {
    std::filesystem::remove(cache.getPathFromUuid(irradianceUuid));
    auto res = cache.request<quoll::EnvironmentAsset>(asset.uuid);

    EXPECT_FALSE(res);
    EXPECT_EQ(cache.getRegistry().count<quoll::TextureAsset>(), 0);
  }

  {
    cache.createFromSource<quoll::TextureAsset>(
        FixturesPath / "1x1-cubemap.ktx", irradianceUuid);
    std::filesystem::remove(cache.getPathFromUuid(specularUuid));

    auto res = cache.request<quoll::EnvironmentAsset>(asset.uuid);

    EXPECT_FALSE(res);
    EXPECT_EQ(cache.getRegistry().count<quoll::TextureAsset>(), 0);
  }
}

TEST_F(AssetCacheEnvironmentTest,
       LoadsEnvironmentAssetWithTexturesIfTexturesAreNotLoaded) {
  auto irradianceMap = cache.request<quoll::TextureAsset>(irradianceUuid);
  auto specularMap = cache.request<quoll::TextureAsset>(specularUuid);

  ASSERT_TRUE(irradianceMap);
  ASSERT_TRUE(specularMap);

  quoll::AssetData<quoll::EnvironmentAsset> asset{};
  asset.uuid = quoll::Uuid::generate();
  asset.data.irradianceMap = irradianceMap.data();
  asset.data.specularMap = specularMap.data();
  auto createRes = cache.createFromData(asset);

  cache.getRegistry().remove(irradianceMap.data().handle());
  cache.getRegistry().remove(specularMap.data().handle());

  EXPECT_EQ(cache.getRegistry().count<quoll::TextureAsset>(), 0);

  auto res = cache.request<quoll::EnvironmentAsset>(asset.uuid);

  EXPECT_TRUE(res);
  EXPECT_FALSE(res.hasWarnings());

  EXPECT_EQ(cache.getRegistry().count<quoll::TextureAsset>(), 2);
  EXPECT_GT(cache.getRegistry().count<quoll::EnvironmentAsset>(), 0);

  auto environment = res.data();
  EXPECT_NE(environment->irradianceMap,
            quoll::AssetHandle<quoll::TextureAsset>());
  EXPECT_NE(environment->specularMap,
            quoll::AssetHandle<quoll::TextureAsset>());
}

TEST_F(AssetCacheEnvironmentTest,
       LoadsEnvironmentAssetWithExistingTexturesIfTexturesAreLoaded) {
  auto irradianceMap = cache.request<quoll::TextureAsset>(irradianceUuid);
  auto specularMap = cache.request<quoll::TextureAsset>(specularUuid);

  ASSERT_TRUE(irradianceMap);
  ASSERT_TRUE(specularMap);

  quoll::AssetData<quoll::EnvironmentAsset> asset{};
  asset.uuid = quoll::Uuid::generate();
  asset.data.irradianceMap = irradianceMap.data();
  asset.data.specularMap = specularMap.data();
  auto createRes = cache.createFromData(asset);

  EXPECT_EQ(cache.getRegistry().count<quoll::TextureAsset>(), 2);

  auto res = cache.request<quoll::EnvironmentAsset>(asset.uuid);

  EXPECT_TRUE(res);
  EXPECT_FALSE(res.hasWarnings());

  EXPECT_EQ(cache.getRegistry().count<quoll::TextureAsset>(), 2);
  EXPECT_GT(cache.getRegistry().count<quoll::EnvironmentAsset>(), 0);

  auto environment = res.data();

  EXPECT_EQ(environment->irradianceMap, irradianceMap);
  EXPECT_EQ(environment->specularMap, specularMap);
}
