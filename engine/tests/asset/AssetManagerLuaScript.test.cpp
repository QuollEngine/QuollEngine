#include "liquid/core/Base.h"
#include <random>

#include "liquid/core/Version.h"
#include "liquid/asset/AssetManager.h"
#include "liquid/asset/AssetFileHeader.h"
#include "liquid/asset/InputBinaryStream.h"

#include <gtest/gtest.h>
#include <glm/gtx/string_cast.hpp>

class AssetManagerTest : public ::testing::Test {
public:
  AssetManagerTest() : manager(std::filesystem::current_path()) {}

  liquid::AssetManager manager;
};

using AssetManagerDeathTest = AssetManagerTest;

TEST_F(AssetManagerTest, ReturnsErrorIfFileCannotBeOpened) {
  auto scriptPath = std::filesystem::current_path() / "non-existent-script.lua";

  auto result = manager.loadLuaScriptFromFile(scriptPath);
  EXPECT_TRUE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_FALSE(result.hasData());
}

TEST_F(AssetManagerTest, LoadsLuaScriptIntoRegistry) {
  auto scriptPath = std::filesystem::current_path() / "component-script.lua";

  auto result = manager.loadLuaScriptFromFile(scriptPath);
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  auto &script = manager.getRegistry().getLuaScripts().getAsset(handle);
  EXPECT_EQ(script.name, "component-script.lua");
  EXPECT_EQ(script.path, manager.getAssetsPath() / script.name);
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

TEST_F(AssetManagerTest, UpdatesExistingLuaScriptIfHandleExists) {
  // Load script and create handle
  auto scriptPath = std::filesystem::current_path() / "component-script.lua";
  auto result = manager.loadLuaScriptFromFile(scriptPath);
  EXPECT_FALSE(result.hasError());
  EXPECT_FALSE(result.hasWarnings());
  EXPECT_TRUE(result.hasData());

  auto handle = result.getData();

  // Load script to update the handle
  auto scriptPath2 = std::filesystem::current_path() / "component-script-2.lua";
  manager.loadLuaScriptFromFile(scriptPath2, handle);

  auto &script = manager.getRegistry().getLuaScripts().getAsset(handle);
  EXPECT_EQ(script.name, "component-script-2.lua");
  EXPECT_EQ(script.path, manager.getAssetsPath() / script.name);
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

TEST_F(AssetManagerDeathTest, UpdateFailsIfProvidedHandleDoesNotExist) {
  // Load script and create handle
  auto scriptPath = std::filesystem::current_path() / "component-script.lua";
  EXPECT_DEATH(manager.loadLuaScriptFromFile(scriptPath,
                                             liquid::LuaScriptAssetHandle{25}),
               ".*");
}
