#pragma once

#include "liquid/asset/AssetCache.h"
#include "liquid/scripting/ScriptingSystem.h"

/**
 * @brief Test base class for Lua scripting interfaces
 */
class LuaScriptingInterfaceTestBase : public ::testing::Test {
  static const liquid::String ScriptName;

public:
  /**
   * @brief Create test base
   *
   * @param scriptName Script name
   */
  LuaScriptingInterfaceTestBase(const liquid::String &scriptName = ScriptName);

  /**
   * @brief Call function
   *
   * @param entity Entity
   * @param functionName Function name
   * @return Lua scope
   */
  liquid::LuaScope &call(liquid::Entity entity,
                         const liquid::String &functionName);

protected:
  liquid::EntityDatabase entityDatabase;
  liquid::EventSystem eventSystem;
  liquid::AssetCache assetCache;
  liquid::ScriptingSystem scriptingSystem;

private:
  liquid::String mScriptName;
};
