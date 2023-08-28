#include "liquid/core/Base.h"
#include <random>

#include "liquid/asset/AssetCache.h"

#include "liquid-tests/Testing.h"

#include "liquid-tests/test-utils/AssetCacheTestBase.h"

class AssetCacheLuaScriptTest : public AssetCacheTestBase {
public:
  liquid::Result<liquid::LuaScriptAssetHandle>
  loadFromSource(liquid::Path sourcePath) {
    auto uuid = liquid::Uuid::generate();
    auto cachePath = cache.createLuaScriptFromSource(sourcePath, uuid);

    return cache.loadLuaScript(uuid);
  }
};

using AssetCacheLuaScriptDeathTest = AssetCacheLuaScriptTest;

TEST_F(AssetCacheLuaScriptTest, CreateLuaScriptFromSource) {
  auto scriptPath = FixturesPath / "script-asset-valid.lua";

  auto uuid = liquid::Uuid::generate();
  auto filePath = cache.createLuaScriptFromSource(scriptPath, uuid);
  EXPECT_TRUE(filePath.hasData());
  EXPECT_FALSE(filePath.hasError());
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.getData().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);

  EXPECT_EQ(meta.type, liquid::AssetType::LuaScript);
  EXPECT_EQ(meta.name, "script-asset-valid.lua");
}

TEST_F(AssetCacheLuaScriptTest, ReturnsErrorIfFileCannotBeOpened) {
  auto result = cache.loadLuaScript(liquid::Uuid::generate());
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheLuaScriptTest, ReturnsErrorIfScriptHasNoStartFunction) {
  auto scriptPath = FixturesPath / "script-asset-no-start.lua";

  auto result = loadFromSource(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheLuaScriptTest, ReturnsErrorIfScriptHasNoUpdateFunction) {
  auto scriptPath = FixturesPath / "script-asset-no-update.lua";

  auto result = loadFromSource(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheLuaScriptTest, ReturnsErrorIfScriptHasSyntaxError) {
  auto scriptPath = FixturesPath / "script-asset-invalid-syntax.lua";

  auto result = loadFromSource(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheLuaScriptTest,
       ReturnsErrorIfScriptRegistersMultipleVariablesWithTheSameName) {
  auto scriptPath = FixturesPath / "script-asset-duplicate-var-names.lua";

  auto result = loadFromSource(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheLuaScriptTest,
       ReturnsErrorIfScriptRegistersVariablesWithInvalidTypes) {
  auto scriptPath = FixturesPath / "script-asset-invalid-var-type.lua";

  auto result = loadFromSource(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheLuaScriptTest, LoadsLuaScriptIntoRegistry) {
  auto scriptPath = FixturesPath / "script-asset-valid.lua";

  auto result = loadFromSource(scriptPath);
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  auto &script = cache.getRegistry().getLuaScripts().getAsset(handle);
  script.name = "script-asset-valid.lua";
  EXPECT_EQ(script.name, "script-asset-valid.lua");
  EXPECT_EQ(script.type, liquid::AssetType::LuaScript);
  EXPECT_EQ(script.data.variables.size(), 2);

  EXPECT_EQ(script.data.variables.at("string_value").type,
            liquid::LuaScriptVariableType::String);
  EXPECT_EQ(script.data.variables.at("prefab_value").type,
            liquid::LuaScriptVariableType::AssetPrefab);

  std::ifstream file(scriptPath);

  EXPECT_TRUE(file.good());

  std::ostringstream ss;
  ss << file.rdbuf();
  const std::string &s = ss.str();
  std::vector<char> bytes(s.begin(), s.end());
  file.close();

  liquid::String contents(bytes.begin(), bytes.end());
  liquid::String scriptContents(script.data.bytes.begin(),
                                script.data.bytes.end());
  EXPECT_EQ(scriptContents, contents);
}

TEST_F(AssetCacheLuaScriptTest, UpdatesExistingLuaScriptIfHandleExists) {
  auto uuid1 = liquid::Uuid::generate();
  auto filePath = cache.createLuaScriptFromSource(
      FixturesPath / "component-script.lua", uuid1);

  auto result = cache.loadLuaScript(uuid1);
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  auto uuid2 = liquid::Uuid::generate();
  auto filePath2 = cache.createLuaScriptFromSource(
      FixturesPath / "component-script-2.lua", uuid2);
  cache.loadLuaScript(uuid2, handle);

  const auto &script = cache.getRegistry().getLuaScripts().getAsset(handle);
  EXPECT_EQ(script.type, liquid::AssetType::LuaScript);

  std::ifstream file(script.path);

  EXPECT_TRUE(file.good());

  std::ostringstream ss;
  ss << file.rdbuf();
  const std::string &s = ss.str();
  std::vector<char> bytes(s.begin(), s.end());
  file.close();

  liquid::String contents(bytes.begin(), bytes.end());
  liquid::String scriptContents(script.data.bytes.begin(),
                                script.data.bytes.end());
  EXPECT_EQ(scriptContents, contents);
}

TEST_F(AssetCacheLuaScriptDeathTest, UpdateFailsIfProvidedHandleDoesNotExist) {
  auto uuid = liquid::Uuid::generate();
  auto filePath = cache.createLuaScriptFromSource(
      FixturesPath / "script-asset-valid.lua", uuid);

  EXPECT_DEATH(cache.loadLuaScript(uuid, liquid::LuaScriptAssetHandle{25}),
               ".*");
}
