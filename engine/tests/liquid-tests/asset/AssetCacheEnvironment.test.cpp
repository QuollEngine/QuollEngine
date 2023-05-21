#include "liquid/core/Base.h"
#include "liquid/core/Version.h"
#include "liquid/asset/AssetCache.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid-tests/Testing.h"

static liquid::Path EnvDirectory = FixturesPath / "test-env";

class AssetCacheTest : public ::testing::Test {
public:
  AssetCacheTest() : cache(FixturesPath) {}

  void SetUp() override {
    std::filesystem::create_directory(EnvDirectory);
    std::filesystem::copy(FixturesPath / "1x1-cubemap.ktx",
                          EnvDirectory / "irradiance.ktx");
    std::filesystem::copy(FixturesPath / "1x1-cubemap.ktx",
                          EnvDirectory / "specular.ktx");
    std::filesystem::copy(FixturesPath / "1x1-2d.ktx",
                          EnvDirectory / "brdf.ktx");
  }

  void TearDown() override { std::filesystem::remove_all(EnvDirectory); }

public:
  liquid::AssetCache cache;
};

TEST_F(AssetCacheTest, CreatesEnvironmentFileFromEnvironmentAsset) {
  auto irradianceMap =
      cache.loadTextureFromFile(EnvDirectory / "irradiance.ktx").getData();
  auto specularMap =
      cache.loadTextureFromFile(EnvDirectory / "specular.ktx").getData();

  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.name = "environment.lqenv";
  asset.path = FixturesPath / "environment.lqenv";
  asset.data.irradianceMap = irradianceMap;
  asset.data.specularMap = specularMap;

  auto res = cache.createEnvironmentFromAsset(asset);
  EXPECT_TRUE(res.hasData());
  EXPECT_FALSE(res.hasError());
  EXPECT_FALSE(res.hasWarnings());

  liquid::InputBinaryStream file(res.getData());
  EXPECT_TRUE(file.good());
  liquid::AssetFileHeader header;
  liquid::String magic(liquid::AssetFileMagicLength, '$');
  file.read(magic.data(), magic.length());
  file.read(header.version);
  file.read(header.type);
  EXPECT_EQ(magic, header.magic);
  EXPECT_EQ(header.version, liquid::createVersion(0, 1));
  EXPECT_EQ(header.type, liquid::AssetType::Environment);

  liquid::String irradianceMapPath;
  file.read(irradianceMapPath);

  liquid::String specularMapPath;
  file.read(specularMapPath);

  EXPECT_EQ(irradianceMapPath, "test-env/irradiance.ktx");
  EXPECT_EQ(specularMapPath, "test-env/specular.ktx");
}

TEST_F(AssetCacheTest,
       DoesNotLoadEnvironmentAssetIfEnvironmentTexturesCouldNotBeLoaded) {
  auto irradianceMap =
      cache.loadTextureFromFile(EnvDirectory / "irradiance.ktx").getData();
  auto specularMap =
      cache.loadTextureFromFile(EnvDirectory / "specular.ktx").getData();

  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.name = "environment.lqenv";
  asset.path = FixturesPath / "environment.lqenv";
  asset.data.irradianceMap = irradianceMap;
  asset.data.specularMap = specularMap;
  auto createRes = cache.createEnvironmentFromAsset(asset);

  cache.getRegistry().getTextures().deleteAsset(irradianceMap);
  cache.getRegistry().getTextures().deleteAsset(specularMap);

  {
    std::filesystem::remove(EnvDirectory / "irradiance.ktx");
    auto res = cache.loadEnvironmentFromFile(createRes.getData());

    EXPECT_TRUE(res.hasError());
    EXPECT_TRUE(cache.getRegistry().getTextures().getAssets().empty());
  }

  {
    std::filesystem::copy(FixturesPath / "1x1-cubemap.ktx",
                          EnvDirectory / "irradiance.ktx");
    std::filesystem::remove(EnvDirectory / "specular.ktx");
    auto res = cache.loadEnvironmentFromFile(createRes.getData());

    EXPECT_TRUE(res.hasError());
    EXPECT_TRUE(cache.getRegistry().getTextures().getAssets().empty());
  }
}

TEST_F(AssetCacheTest,
       LoadsEnvironmentAssetWithTexturesIfTexturesAreNotLoaded) {
  auto irradianceMap =
      cache.loadTextureFromFile(EnvDirectory / "irradiance.ktx").getData();
  auto specularMap =
      cache.loadTextureFromFile(EnvDirectory / "specular.ktx").getData();

  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.name = "environment.lqenv";
  asset.path = FixturesPath / "environment.lqenv";
  asset.data.irradianceMap = irradianceMap;
  asset.data.specularMap = specularMap;
  auto createRes = cache.createEnvironmentFromAsset(asset);

  cache.getRegistry().getTextures().deleteAsset(irradianceMap);
  cache.getRegistry().getTextures().deleteAsset(specularMap);

  EXPECT_TRUE(cache.getRegistry().getTextures().getAssets().empty());

  auto res = cache.loadEnvironmentFromFile(createRes.getData());

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

TEST_F(AssetCacheTest,
       LoadsEnvironmentAssetWithExistingTexturesIfTexturesAreLoaded) {
  auto irradianceMap =
      cache.loadTextureFromFile(EnvDirectory / "irradiance.ktx").getData();
  auto specularMap =
      cache.loadTextureFromFile(EnvDirectory / "specular.ktx").getData();

  liquid::AssetData<liquid::EnvironmentAsset> asset{};
  asset.name = "environment.lqenv";
  asset.path = FixturesPath / "environment.lqenv";
  asset.data.irradianceMap = irradianceMap;
  asset.data.specularMap = specularMap;
  auto createRes = cache.createEnvironmentFromAsset(asset);

  EXPECT_EQ(cache.getRegistry().getTextures().getAssets().size(), 2);

  auto res = cache.loadEnvironmentFromFile(createRes.getData());

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
