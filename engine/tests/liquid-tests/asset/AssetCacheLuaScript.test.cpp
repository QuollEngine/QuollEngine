#include "liquid/core/Base.h"
#include <random>

#include "liquid/asset/AssetCache.h"

#include "liquid-tests/Testing.h"

class AssetCacheTest : public ::testing::Test {
public:
  AssetCacheTest() : cache(FixturesPath) {}

  liquid::AssetCache cache;
};

using AssetCacheDeathTest = AssetCacheTest;

TEST_F(AssetCacheTest, ReturnsErrorIfFileCannotBeOpened) {
  auto scriptPath = FixturesPath / "non-existent-script.lua";

  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheTest, ReturnsErrorIfScriptHasNoStartFunction) {
  auto scriptPath = FixturesPath / "script-asset-no-start.lua";

  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheTest, ReturnsErrorIfScriptHasNoUpdateFunction) {
  auto scriptPath = FixturesPath / "script-asset-no-update.lua";

  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheTest, ReturnsErrorIfScriptHasSyntaxError) {
  auto scriptPath = FixturesPath / "script-asset-invalid-syntax.lua";

  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheTest,
       ReturnsErrorIfScriptRegistersMultipleVariablesWithTheSameName) {
  auto scriptPath = FixturesPath / "script-asset-duplicate-var-names.lua";

  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheTest, ReturnsErrorIfScriptRegistersVariablesWithInvalidTypes) {
  auto scriptPath = FixturesPath / "script-asset-invalid-var-type.lua";

  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheTest, LoadsLuaScriptIntoRegistry) {
  auto scriptPath = FixturesPath / "script-asset-valid.lua";

  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  auto &script = cache.getRegistry().getLuaScripts().getAsset(handle);
  EXPECT_EQ(script.name, "script-asset-valid.lua");
  EXPECT_EQ(script.path, cache.getAssetsPath() / script.name);
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

TEST_F(AssetCacheTest, UpdatesExistingLuaScriptIfHandleExists) {
  // Load script and create handle
  auto scriptPath = FixturesPath / "component-script.lua";
  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  // Load script to update the handle
  auto scriptPath2 = FixturesPath / "component-script-2.lua";
  cache.loadLuaScriptFromFile(scriptPath2, handle);

  auto &script = cache.getRegistry().getLuaScripts().getAsset(handle);
  EXPECT_EQ(script.name, "component-script-2.lua");
  EXPECT_EQ(script.path, cache.getAssetsPath() / script.name);
  EXPECT_EQ(script.type, liquid::AssetType::LuaScript);

  std::ifstream file(scriptPath2);

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

TEST_F(AssetCacheDeathTest, UpdateFailsIfProvidedHandleDoesNotExist) {
  // Load script and create handle
  auto scriptPath = FixturesPath / "script-asset-valid.lua";
  EXPECT_DEATH(
      cache.loadLuaScriptFromFile(scriptPath, liquid::LuaScriptAssetHandle{25}),
      ".*");
}
