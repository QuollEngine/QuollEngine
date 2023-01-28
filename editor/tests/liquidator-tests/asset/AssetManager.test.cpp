#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/yaml/Yaml.h"

#include "liquidator-tests/Testing.h"
#include "liquidator-tests/mocks/MockRenderDevice.h"

#include "liquidator/asset/AssetManager.h"

#include <cryptopp/sha.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>

namespace fs = std::filesystem;

static const liquid::Path AssetsPath = fs::current_path() / "assets";
static const liquid::Path CachePath = fs::current_path() / "cache";
static const liquid::Path FixturesPath = fs::current_path() / "fixtures";
static const liquid::Path TempPath = fs::current_path() / "temp";
static const liquid::Path InnerPathInAssets =
    AssetsPath / "inner-1" / "inner-2";
static const liquid::Path InnerPathInCache = CachePath / "inner-1" / "inner-2";

class AssetManagerTest : public ::testing::Test {
public:
  AssetManagerTest() : manager(AssetsPath, CachePath, false) {}

  void SetUp() override {
    fs::create_directory(AssetsPath);
    fs::create_directory(CachePath);
    fs::create_directory(TempPath);
  }

  void TearDown() override {
    fs::remove_all(AssetsPath);
    fs::remove_all(CachePath);
    fs::remove_all(TempPath);
  }

  void createEmptyFile(liquid::Path path) {
    std::ofstream stream(path);
    stream.close();
  }

public:
  liquid::editor::AssetManager manager;
};

class AssetTest : public AssetManagerTest,
                  public ::testing::WithParamInterface<
                      std::tuple<liquid::String, liquid ::String>> {};

template <class T>
std::vector<std::tuple<liquid::String, liquid::String>>
mapExtensions(const std::vector<liquid::String> &extensions, T &&fn) {
  std::vector<std::tuple<liquid::String, liquid::String>> temp(
      extensions.size());
  std::transform(extensions.begin(), extensions.end(), temp.begin(),
                 [&fn](auto str) {
                   return std::tuple{str, fn(str)};
                 });
  return temp;
}

#define InitAssetTestSuite(SuiteName, Extensions, Fn)                          \
  INSTANTIATE_TEST_SUITE_P(                                                    \
      SuiteName, AssetTest, testing::ValuesIn(mapExtensions(Extensions, Fn)),  \
      [](const ::testing::TestParamInfo<AssetTest::ParamType> &info) {         \
        return std::get<0>(info.param);                                        \
      })

TEST_F(AssetManagerTest, SetsProvidedAssetsAndCachePathsOnConstruct) {
  EXPECT_EQ(manager.getAssetsPath(),
            std::filesystem::current_path() / "assets");
  EXPECT_EQ(manager.getCachePath(), std::filesystem::current_path() / "cache");
}

TEST_F(AssetManagerTest, CreatesScriptFileAndLoadsIt) {
  auto handle = manager.createLuaScript(InnerPathInAssets / "test");

  EXPECT_TRUE(handle.hasData());
  EXPECT_EQ(handle.getData(), InnerPathInAssets / "test.lua");
  EXPECT_TRUE(fs::exists(InnerPathInAssets / ("test.lua")));
}

TEST_F(AssetManagerTest,
       ValidateAndPreloadDeletesCacheFileIfAssetFileDoesNotExist) {
  fs::create_directories(InnerPathInCache);

  auto texturePath = InnerPathInCache / "test.ktx2";
  auto textureHashPath = InnerPathInCache / "test.ktx2.lqhash";

  auto engineAssetPathStr =
      std::filesystem::relative(texturePath, manager.getCachePath()).string();

  std::replace(engineAssetPathStr.begin(), engineAssetPathStr.end(), '\\', '/');

  YAML::Node node;
  node["engineAssetPath"] = engineAssetPathStr;

  std::ofstream stream(textureHashPath);
  stream << node;
  stream.close();

  MockRenderDevice device;
  liquid::RenderStorage renderStorage(&device);

  createEmptyFile(texturePath);

  EXPECT_TRUE(fs::exists(textureHashPath));
  EXPECT_TRUE(fs::exists(texturePath));

  manager.validateAndPreloadAssets(renderStorage);

  EXPECT_FALSE(fs::exists(textureHashPath));
  EXPECT_FALSE(fs::exists(texturePath));
}

TEST_F(AssetManagerTest,
       ValidateAndPreloadDeletesPrefabDirectoryIfAssetFileDoesNotExist) {
  auto prefabBasePath = InnerPathInCache / "test-prefab";
  auto prefabPath = prefabBasePath / "test.lqprefab";
  auto prefabMeshPath = prefabBasePath / "test.lqmesh ";
  auto prefabHashPath = InnerPathInCache / "test-prefab.lqhash";

  fs::create_directories(prefabBasePath);

  auto engineAssetPathStr =
      std::filesystem::relative(prefabPath, manager.getCachePath()).string();

  std::replace(engineAssetPathStr.begin(), engineAssetPathStr.end(), '\\', '/');

  YAML::Node node;
  node["engineAssetPath"] = engineAssetPathStr;

  std::ofstream stream(prefabHashPath);
  stream << node;
  stream.close();

  createEmptyFile(prefabPath);
  createEmptyFile(prefabMeshPath);

  MockRenderDevice device;
  liquid::RenderStorage renderStorage(&device);

  EXPECT_TRUE(fs::exists(prefabPath));
  EXPECT_TRUE(fs::exists(prefabMeshPath));
  EXPECT_TRUE(fs::exists(prefabHashPath));

  manager.validateAndPreloadAssets(renderStorage);

  EXPECT_FALSE(fs::exists(prefabPath));
  EXPECT_FALSE(fs::exists(prefabMeshPath));
  EXPECT_FALSE(fs::exists(prefabHashPath));
  EXPECT_FALSE(fs::exists(prefabBasePath));
}

TEST_P(AssetTest, FailedImportDoesNotCreateAssetInCache) {
  auto originalExtension = std::get<0>(GetParam());
  // Lua scripts are not compiled at load stage
  // They are only compiled on start by the scripting
  // manager
  if (originalExtension == "lua") {
    return;
  }

  auto cacheExtension = std::get<1>(GetParam());
  auto originalFilename = "empty-asset." + originalExtension;
  auto cacheFilename = "empty-asset." + cacheExtension;

  std::ofstream stream(TempPath / originalFilename, std::ios::binary);
  stream.close();

  auto res = manager.importAsset(TempPath / originalFilename, AssetsPath);

  EXPECT_TRUE(res.hasError());
  EXPECT_FALSE(fs::exists(AssetsPath / originalFilename));
  EXPECT_FALSE(fs::exists(CachePath / cacheFilename));
}

TEST_P(AssetTest, FailedImportDoesNotDeleteExistingSubdirectories) {
  auto originalExtension = std::get<0>(GetParam());
  // Lua scripts are not compiled at load stage
  // They are only compiled on start by the scripting
  // manager
  if (originalExtension == "lua") {
    return;
  }

  auto cacheExtension = std::get<1>(GetParam());
  auto emptyFilename = "empty-asset." + originalExtension;
  auto validOriginalFilename = "valid-asset." + originalExtension;
  auto validEngineFilename = "valid-asset." + cacheExtension;

  std::ofstream stream(TempPath / emptyFilename, std::ios::binary);
  stream.close();

  ASSERT_TRUE(
      manager
          .importAsset(FixturesPath / validOriginalFilename, InnerPathInAssets)
          .hasData());

  {
    auto subdirectory = InnerPathInAssets;
    auto cacheSubdirectory = InnerPathInCache;

    auto res = manager.importAsset(TempPath / emptyFilename, subdirectory);

    EXPECT_TRUE(res.hasError());
    EXPECT_TRUE(fs::exists(subdirectory));
    EXPECT_TRUE(fs::exists(cacheSubdirectory));
    EXPECT_TRUE(fs::exists(InnerPathInAssets / validOriginalFilename));
    EXPECT_TRUE(fs::exists(InnerPathInCache / validEngineFilename));
  }

  {
    auto subdirectory = InnerPathInAssets.parent_path();
    auto cacheSubdirectory = InnerPathInCache.parent_path();

    auto res = manager.importAsset(TempPath / emptyFilename, subdirectory);

    EXPECT_TRUE(res.hasError());
    EXPECT_TRUE(fs::exists(subdirectory));
    EXPECT_TRUE(fs::exists(cacheSubdirectory));
    EXPECT_TRUE(fs::exists(InnerPathInAssets / validOriginalFilename));
    EXPECT_TRUE(fs::exists(InnerPathInCache / validEngineFilename));
  }
}

TEST_P(AssetTest, FailedImportDoesNotCreateNewSubdirectories) {
  auto originalExtension = std::get<0>(GetParam());
  // Lua scripts are not compiled at load stage
  // They are only compiled on start by the scripting
  // manager
  if (originalExtension == "lua") {
    return;
  }

  auto cacheExtension = std::get<1>(GetParam());
  auto emptyFilename = "empty-asset." + originalExtension;
  auto validOriginalFilename = "valid-asset." + originalExtension;
  auto validEngineFilename = "valid-asset." + cacheExtension;

  std::ofstream stream(TempPath / emptyFilename, std::ios::binary);
  stream.close();

  ASSERT_TRUE(
      manager
          .importAsset(FixturesPath / validOriginalFilename, InnerPathInAssets)
          .hasData());

  {
    auto subdirectory = InnerPathInAssets / "test-inner";
    auto cacheSubdirectory = InnerPathInCache / "test-inner";

    auto res = manager.importAsset(TempPath / emptyFilename, subdirectory);

    EXPECT_TRUE(res.hasError());
    EXPECT_FALSE(fs::exists(subdirectory));
    EXPECT_FALSE(fs::exists(cacheSubdirectory));
  }
}

TEST_P(AssetTest, ImportCopiesSourceToAssets) {
  auto originalExtension = std::get<0>(GetParam());
  auto originalFilename = "valid-asset." + originalExtension;

  auto res = manager.importAsset(FixturesPath / originalFilename, AssetsPath);

  EXPECT_TRUE(res.hasData());
  EXPECT_TRUE(fs::exists(AssetsPath / originalFilename));
}

TEST_P(AssetTest, ImportModifiesTheNameAndCopiesSourceToAssetsIfDuplicate) {
  auto originalExtension = std::get<0>(GetParam());
  auto originalFilename = "valid-asset." + originalExtension;

  {
    auto res = manager.importAsset(FixturesPath / originalFilename, AssetsPath);
    EXPECT_TRUE(res.hasData());
    EXPECT_TRUE(fs::exists(AssetsPath / originalFilename));
  }

  for (uint32_t i = 0; i < 10; ++i) {
    auto originalDuplicateName =
        "valid-asset-" + std::to_string(i + 1) + "." + originalExtension;
    auto res = manager.importAsset(FixturesPath / originalFilename, AssetsPath);
    EXPECT_TRUE(res.hasData());
    EXPECT_TRUE(fs::exists(AssetsPath / originalDuplicateName))
        << originalDuplicateName << " does not exist";
  }
}

TEST_P(AssetTest, ImportCreatesAssetInCache) {
  auto originalExtension = std::get<0>(GetParam());
  auto cacheExtension = std::get<1>(GetParam());
  auto originalFilename = "valid-asset." + originalExtension;
  auto cacheFilename = "valid-asset." + cacheExtension;

  auto res = manager.importAsset(FixturesPath / originalFilename, AssetsPath);

  EXPECT_TRUE(res.hasData());
  EXPECT_TRUE(fs::exists(CachePath / cacheFilename));
  EXPECT_EQ(manager.findEngineAssetPath(AssetsPath / originalFilename),
            CachePath / cacheFilename);
}

TEST_P(AssetTest, ImportMirrorsCacheSubdirectoriesWithAssets) {
  auto originalExtension = std::get<0>(GetParam());
  auto cacheExtension = std::get<1>(GetParam());
  auto originalFilename = "valid-asset." + originalExtension;
  auto cacheFilename = "valid-asset." + cacheExtension;

  auto res =
      manager.importAsset(FixturesPath / originalFilename, InnerPathInAssets);

  EXPECT_TRUE(res.hasData());
  EXPECT_TRUE(fs::exists(InnerPathInCache / cacheFilename));
}

TEST_P(AssetTest, ImportCreatesHashFileInCache) {
  auto originalExtension = std::get<0>(GetParam());
  auto cacheExtension = std::get<1>(GetParam());
  auto originalFilename = "valid-asset." + originalExtension;
  auto cacheFilename = "valid-asset." + cacheExtension;
  auto hashFilename = originalFilename + ".lqhash";

  auto res =
      manager.importAsset(FixturesPath / originalFilename, InnerPathInAssets);

  EXPECT_TRUE(res.hasData());
  EXPECT_TRUE(fs::exists(InnerPathInAssets / originalFilename));
  EXPECT_TRUE(fs::exists(InnerPathInCache / hashFilename));

  std::ifstream stream(InnerPathInCache / hashFilename);
  auto node = YAML::Load(stream);
  auto originalAssetHash = node["originalAssetHash"].as<liquid::String>();
  auto engineAssetHash = node["engineAssetHash"].as<liquid::String>();
  auto engineAssetPathStr = node["engineAssetPath"].as<liquid::String>();
  stream.close();

  auto engineAssetPath = (CachePath / engineAssetPathStr).make_preferred();
  EXPECT_EQ(engineAssetPath, InnerPathInCache / cacheFilename);

  {
    std::ifstream stream(engineAssetPath, std::ios::binary);

    liquid::String string;
    CryptoPP::SHA256 sha256;
    CryptoPP::FileSource source(
        stream, true,
        new CryptoPP::HashFilter(
            sha256,
            new CryptoPP::HexEncoder(new CryptoPP::StringSink(string))));

    EXPECT_EQ(engineAssetHash, string);

    stream.close();
  }

  {
    std::ifstream stream(InnerPathInAssets / originalFilename,
                         std::ios::binary);

    liquid::String string;
    CryptoPP::SHA256 sha256;
    CryptoPP::FileSource source(
        stream, true,
        new CryptoPP::HashFilter(
            sha256,
            new CryptoPP::HexEncoder(new CryptoPP::StringSink(string))));

    EXPECT_EQ(originalAssetHash, string);

    stream.close();
  }
}

InitAssetTestSuite(AssetManagerTexture,
                   liquid::editor::AssetManager::TextureExtensions,
                   [](auto str) { return str + ".ktx2"; });

InitAssetTestSuite(AssetManagerAudio,
                   liquid::editor::AssetManager::AudioExtensions,
                   [](auto str) { return str; });

InitAssetTestSuite(AssetManagerScript,
                   liquid::editor::AssetManager::ScriptExtensions,
                   [](auto str) { return str; });
