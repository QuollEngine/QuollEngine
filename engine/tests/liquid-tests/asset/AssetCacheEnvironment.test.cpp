#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"
#include "liquid-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheEnvironmentTest : public AssetCacheTestBase {
public:
  void SetUp() override {
    AssetCacheTestBase::SetUp();

    irradianceUuid = liquid::Uuid::generate();
    specularUuid = liquid::Uuid::generate();

    cache.createTextureFromSource(FixturesPath / "1x1-cubemap.ktx",
                                  irradianceUuid);
    cache.createTextureFromSource(FixturesPath / "1x1-cubemap.ktx",
                                  specularUuid);
  }

  liquid::Uuid irradianceUuid;
  liquid::Uuid specularUuid;
};

TEST_F(AssetCacheEnvironmentTest, CreatesEnvironmentFileFromEnvironmentAsset) {
  auto irradianceMap = cache.loadTexture(irradianceUuid).getData();
  auto specularMap = cache.loadTexture(specularUuid).getData();

  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.name = "env-0";
  asset.uuid = liquid::Uuid::generate();
  asset.path = FixturesPath / "environment";
  asset.data.irradianceMap = irradianceMap;
  asset.data.specularMap = specularMap;

  auto filePath = cache.createEnvironmentFromAsset(asset);
  EXPECT_TRUE(filePath.hasData());
  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  liquid::InputBinaryStream file(filePath.getData());
  EXPECT_TRUE(file.good());
  liquid::AssetFileHeader header;
  file.read(header);
  EXPECT_EQ(header.magic, header.MagicConstant);
  EXPECT_EQ(header.type, liquid::AssetType::Environment);
  EXPECT_EQ(header.name, asset.name);

  liquid::Uuid readIrradianceUuid;
  file.read(readIrradianceUuid);

  liquid::Uuid readSpecularUuid;
  file.read(readSpecularUuid);

  EXPECT_EQ(readIrradianceUuid, irradianceUuid);
  EXPECT_EQ(readSpecularUuid, specularUuid);

  EXPECT_FALSE(std::filesystem::exists(
      filePath.getData().replace_extension("assetmeta")));
}

TEST_F(AssetCacheEnvironmentTest,
       DoesNotLoadEnvironmentAssetIfEnvironmentTexturesCouldNotBeLoaded) {
  auto irradianceMap = cache.loadTexture(irradianceUuid).getData();
  auto specularMap = cache.loadTexture(specularUuid).getData();

  liquid::AssetData<liquid::EnvironmentAsset> asset{};

  asset.uuid = liquid::Uuid::generate();
  asset.path = FixturesPath / "environment.lqenv";
  asset.data.irradianceMap = irradianceMap;
  asset.data.specularMap = specularMap;
  auto createRes = cache.createEnvironmentFromAsset(asset);

  cache.getRegistry().getTextures().deleteAsset(irradianceMap);
  cache.getRegistry().getTextures().deleteAsset(specularMap);

  {
    std::filesystem::remove(cache.getPathFromUuid(irradianceUuid));
    auto res = cache.loadEnvironment(asset.uuid);

    EXPECT_TRUE(res.hasError());
    EXPECT_TRUE(cache.getRegistry().getTextures().getAssets().empty());
  }

  {
    cache.createTextureFromSource(FixturesPath / "1x1-cubemap.ktx",
                                  irradianceUuid);
    std::filesystem::remove(cache.getPathFromUuid(specularUuid));

    auto res = cache.loadEnvironment(asset.uuid);

    EXPECT_TRUE(res.hasError());
    EXPECT_TRUE(cache.getRegistry().getTextures().getAssets().empty());
  }
}

TEST_F(AssetCacheEnvironmentTest,
       LoadsEnvironmentAssetWithTexturesIfTexturesAreNotLoaded) {
  auto irradianceMap = cache.loadTexture(irradianceUuid).getData();
  auto specularMap = cache.loadTexture(specularUuid).getData();

  liquid::AssetData<liquid::EnvironmentAsset> asset{};

  asset.path = FixturesPath / "environment.lqenv";
  asset.uuid = liquid::Uuid::generate();
  asset.data.irradianceMap = irradianceMap;
  asset.data.specularMap = specularMap;
  auto createRes = cache.createEnvironmentFromAsset(asset);

  cache.getRegistry().getTextures().deleteAsset(irradianceMap);
  cache.getRegistry().getTextures().deleteAsset(specularMap);

  EXPECT_TRUE(cache.getRegistry().getTextures().getAssets().empty());

  auto res = cache.loadEnvironment(asset.uuid);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  EXPECT_EQ(cache.getRegistry().getTextures().getAssets().size(), 2);
  EXPECT_FALSE(cache.getRegistry().getEnvironments().getAssets().empty());

  const auto &environment =
      cache.getRegistry().getEnvironments().getAsset(res.getData());
  EXPECT_NE(environment.data.irradianceMap, liquid::TextureAssetHandle::Null);
  EXPECT_NE(environment.data.specularMap, liquid::TextureAssetHandle::Null);
}

TEST_F(AssetCacheEnvironmentTest,
       LoadsEnvironmentAssetWithExistingTexturesIfTexturesAreLoaded) {
  auto irradianceMap = cache.loadTexture(irradianceUuid).getData();
  auto specularMap = cache.loadTexture(specularUuid).getData();

  liquid::AssetData<liquid::EnvironmentAsset> asset{};

  asset.uuid = liquid::Uuid::generate();
  asset.path = FixturesPath / "environment.lqenv";
  asset.data.irradianceMap = irradianceMap;
  asset.data.specularMap = specularMap;
  auto createRes = cache.createEnvironmentFromAsset(asset);

  EXPECT_EQ(cache.getRegistry().getTextures().getAssets().size(), 2);

  auto res = cache.loadEnvironment(asset.uuid);

  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  EXPECT_EQ(cache.getRegistry().getTextures().getAssets().size(), 2);

  EXPECT_FALSE(cache.getRegistry().getEnvironments().getAssets().empty());

  const auto &environment =
      cache.getRegistry().getEnvironments().getAsset(res.getData());

  EXPECT_EQ(environment.data.irradianceMap, irradianceMap);
  EXPECT_EQ(environment.data.specularMap, specularMap);
}
