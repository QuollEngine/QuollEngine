#include "liquid/core/Base.h"
#include <random>

#include "liquid/asset/AssetCache.h"

#include "liquid-tests/Testing.h"

class AssetCacheTest : public ::testing::Test {
public:
  AssetCacheTest() : cache(std::filesystem::current_path()) {}

  liquid::AssetCache cache;
};

using AssetCacheDeathTest = AssetCacheTest;

TEST_F(AssetCacheTest, ReturnsErrorIfFileCannotBeOpened) {
  auto scriptPath = std::filesystem::current_path() / "non-existent-script.lua";

  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetCacheTest, LoadsLuaScriptIntoRegistry) {
  auto scriptPath = std::filesystem::current_path() / "component-script.lua";

  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  auto &script = cache.getRegistry().getLuaScripts().getAsset(handle);
  EXPECT_EQ(script.name, "component-script.lua");
  EXPECT_EQ(script.path, cache.getAssetsPath() / script.name);
  EXPECT_EQ(script.type, liquid::AssetType::LuaScript);

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
  auto scriptPath = std::filesystem::current_path() / "component-script.lua";
  auto result = cache.loadLuaScriptFromFile(scriptPath);
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  // Load script to update the handle
  auto scriptPath2 = std::filesystem::current_path() / "component-script-2.lua";
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
  auto scriptPath = std::filesystem::current_path() / "component-script.lua";
  EXPECT_DEATH(
      cache.loadLuaScriptFromFile(scriptPath, liquid::LuaScriptAssetHandle{25}),
      ".*");
}
