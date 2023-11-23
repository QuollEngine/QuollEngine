#pragma once

#include "quoll/asset/AssetCache.h"
#include "quoll/lua-scripting/LuaScriptingSystem.h"

#include "TestPhysicsBackend.h"

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
   * @brief Start script
   *
   * @param entity Entity
   * @return Sol state
   */
  sol::state_view start(quoll::Entity entity);

  /**
   * @brief Call function
   *
   * @param entity Entity
   * @param functionName Function name
   * @return Sol state
   */
  sol::state_view call(quoll::Entity entity, const quoll::String &functionName);

  /**
   * @brief Load script
   *
   * @return Script handle
   */
  quoll::LuaScriptAssetHandle loadScript(quoll::String scriptName);

  /**
   * @brief Set up test
   */
  void SetUp() override;

  /**
   * @brief Tear down test
   */
  void TearDown() override;

protected:
  quoll::EntityDatabase entityDatabase;
  quoll::AssetCache assetCache;
  quoll::LuaScriptingSystem scriptingSystem;
  TestPhysicsBackend *physicsBackend = new TestPhysicsBackend;
  quoll::PhysicsSystem physicsSystem;
  quoll::WindowSignals windowSignals;
  quoll::String mScriptName;
};
