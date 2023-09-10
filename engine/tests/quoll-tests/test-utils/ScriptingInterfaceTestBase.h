#pragma once

#include "quoll/asset/AssetCache.h"
#include "quoll/scripting/ScriptingSystem.h"

/**
 * @brief Test base class for Lua scripting interfaces
 */
class LuaScriptingInterfaceTestBase : public ::testing::Test {
  static const quoll::String ScriptName;

public:
  /**
   * @brief Create test base
   *
   * @param scriptName Script name
   */
  LuaScriptingInterfaceTestBase(const quoll::String &scriptName = ScriptName);

  /**
   * @brief Call function
   *
   * @param entity Entity
   * @param functionName Function name
   * @return Lua scope
   */
  quoll::LuaScope &call(quoll::Entity entity,
                        const quoll::String &functionName);

protected:
  quoll::EntityDatabase entityDatabase;
  quoll::EventSystem eventSystem;
  quoll::AssetCache assetCache;
  quoll::ScriptingSystem scriptingSystem;

private:
  quoll::String mScriptName;
};