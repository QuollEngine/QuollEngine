#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/AssetCacheTestBase.h"
#include <random>

class AssetCacheLuaScriptTest : public AssetCacheTestBase {
public:
  quoll::Result<quoll::AssetHandle<quoll::LuaScriptAsset>>
  loadFromSource(quoll::Path sourcePath) {
    auto uuid = quoll::Uuid::generate();
    auto cachePath = cache.createLuaScriptFromSource(sourcePath, uuid);

    return cache.load<quoll::LuaScriptAsset>(uuid);
  }
};

using AssetCacheLuaScriptDeathTest = AssetCacheLuaScriptTest;

TEST_F(AssetCacheLuaScriptTest, CreateLuaScriptFromSource) {
  auto scriptPath = FixturesPath / "script-asset-valid.lua";

  auto uuid = quoll::Uuid::generate();
  auto filePath = cache.createLuaScriptFromSource(scriptPath, uuid);
  EXPECT_TRUE(filePath);
  EXPECT_FALSE(filePath.hasWarnings());

  EXPECT_EQ(filePath.data().filename().string().size(), 38);

  auto meta = cache.getAssetMeta(uuid);

  EXPECT_EQ(meta.type, quoll::AssetType::LuaScript);
  EXPECT_EQ(meta.name, "script-asset-valid.lua");
}

TEST_F(AssetCacheLuaScriptTest, ReturnsErrorIfFileCannotBeOpened) {
  auto result = cache.load<quoll::LuaScriptAsset>(quoll::Uuid::generate());
  EXPECT_FALSE(result);
}

TEST_F(AssetCacheLuaScriptTest, ReturnsErrorIfScriptHasSyntaxError) {
  auto scriptPath = FixturesPath / "script-asset-invalid-syntax.lua";

  auto result = loadFromSource(scriptPath);
  EXPECT_FALSE(result);
}

TEST_F(AssetCacheLuaScriptTest,
       ReturnsErrorIfScriptRegistersMultipleVariablesWithTheSameName) {
  auto scriptPath = FixturesPath / "script-asset-duplicate-var-names.lua";

  auto result = loadFromSource(scriptPath);
  EXPECT_FALSE(result);
}

TEST_F(AssetCacheLuaScriptTest,
       ReturnsErrorIfScriptRegistersVariablesWithInvalidTypes) {
  auto scriptPath = FixturesPath / "script-asset-invalid-var-type.lua";

  auto result = loadFromSource(scriptPath);
  EXPECT_FALSE(result);
}

TEST_F(AssetCacheLuaScriptTest, LoadsLuaScriptIntoRegistry) {
  auto scriptPath = FixturesPath / "script-asset-valid.lua";

  auto result = loadFromSource(scriptPath);
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result);

  auto handle = result;

  {
    auto &script = cache.getRegistry().getMeta(handle.data());
    script.name = "script-asset-valid.lua";
    EXPECT_EQ(script.name, "script-asset-valid.lua");
    EXPECT_EQ(script.type, quoll::AssetType::LuaScript);
  }

  auto &script = cache.getRegistry().get(handle.data());
  EXPECT_EQ(script.variables.size(), 2);
  EXPECT_EQ(script.variables.at("string_value").type,
            quoll::LuaScriptVariableType::String);
  EXPECT_EQ(script.variables.at("prefab_value").type,
            quoll::LuaScriptVariableType::AssetPrefab);

  std::ifstream file(scriptPath);

  EXPECT_TRUE(file.good());

  std::ostringstream ss;
  ss << file.rdbuf();
  const std::string &s = ss.str();
  std::vector<char> bytes(s.begin(), s.end());
  file.close();

  quoll::String contents(bytes.begin(), bytes.end());
  quoll::String scriptContents(script.bytes.begin(), script.bytes.end());
  EXPECT_EQ(scriptContents, contents);
}

TEST_F(AssetCacheLuaScriptTest,
       UpdatesExistingLuaScriptIfAssetWithUuidAlreadyExists) {
  auto uuid1 = quoll::Uuid::generate();
  auto filePath = cache.createLuaScriptFromSource(
      FixturesPath / "component-script.lua", uuid1);

  auto result = cache.load<quoll::LuaScriptAsset>(uuid1);
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result);

  auto handle = result;
  {
    auto &script = cache.getRegistry().getMeta(handle.data());
    EXPECT_EQ(script.type, quoll::AssetType::LuaScript);
    EXPECT_EQ(script.name, "component-script.lua");
  }

  cache.createLuaScriptFromSource(FixturesPath / "component-script-2.lua",
                                  uuid1);

  {
    auto result = cache.load<quoll::LuaScriptAsset>(uuid1);
    EXPECT_EQ(result, handle);
    const auto &scriptMeta = cache.getRegistry().getMeta(handle.data());
    EXPECT_EQ(scriptMeta.type, quoll::AssetType::LuaScript);
    EXPECT_EQ(scriptMeta.name, "component-script-2.lua");

    std::ifstream file(cache.getPathFromUuid(scriptMeta.uuid));

    EXPECT_TRUE(file.good());

    std::ostringstream ss;
    ss << file.rdbuf();
    const std::string &s = ss.str();
    std::vector<char> bytes(s.begin(), s.end());
    file.close();

    const auto &script = cache.getRegistry().get(handle.data());

    quoll::String contents(bytes.begin(), bytes.end());
    quoll::String scriptContents(script.bytes.begin(), script.bytes.end());
    EXPECT_EQ(scriptContents, contents);
  }
}
