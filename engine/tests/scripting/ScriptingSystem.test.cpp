#include "liquid/core/Base.h"
#include "liquid/scripting/ScriptingSystem.h"
#include "liquid/scripting/ScriptingUtils.h"

#include <gtest/gtest.h>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

class ScriptingSystemTest : public ::testing::Test {
public:
  ScriptingSystemTest() : scriptingSystem(entityContext, eventSystem) {}

  liquid::EntityContext entityContext;
  liquid::EventSystem eventSystem;
  liquid::ScriptingSystem scriptingSystem;
};

using ScriptingSystemDeathTest = ScriptingSystemTest;

TEST_F(ScriptingSystemDeathTest, FailsToAddScriptIfFileDoesNotOpen) {
  EXPECT_DEATH(scriptingSystem.addScript("non-existent.lua"), ".*");
}

TEST_F(ScriptingSystemTest, AddsScriptFromFile) {
  auto handle = scriptingSystem.addScript("component-script.lua");
  auto &script = scriptingSystem.getScripts().at(handle);

  auto buffer = liquid::utils::readFileIntoBuffer("component-script.lua");
  liquid::String contents(buffer.begin(), buffer.end());
  liquid::String scriptContents(script.bytes.begin(), script.bytes.end());

  EXPECT_EQ(script.type, liquid::ScriptType::Lua);
  EXPECT_EQ(script.name, "component-script.lua");
  EXPECT_EQ(scriptContents, contents);
}

TEST_F(ScriptingSystemTest, AddsScriptFromMemory) {
  liquid::String scriptText = "print('hello world')";
  std::vector<char> scriptData(scriptText.begin(), scriptText.end());

  auto handle = scriptingSystem.addScript("test-script.lua", scriptData);
  auto &script = scriptingSystem.getScripts().at(handle);

  EXPECT_EQ(script.type, liquid::ScriptType::Lua);
  EXPECT_EQ(script.name, "test-script.lua");
  EXPECT_EQ(liquid::String(script.bytes.begin(), script.bytes.end()),
            scriptText);
}

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnUpdate) {
  auto handle = scriptingSystem.addScript("scripting-system-tester.lua");
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope, nullptr);

  scriptingSystem.start();
  EXPECT_NE(component.scope, nullptr);

  scriptingSystem.update();
  auto *luaScope = static_cast<lua_State *>(component.scope);
  {
    lua_getglobal(luaScope, "value");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 0);
  }
}

TEST_F(ScriptingSystemTest, CallsScriptingUpdateFunctionOnEveryUpdate) {
  auto handle = scriptingSystem.addScript("scripting-system-tester.lua");
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope, nullptr);

  scriptingSystem.start();
  EXPECT_NE(component.scope, nullptr);

  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.update();
  }

  auto *luaScope = static_cast<lua_State *>(component.scope);
  {
    lua_getglobal(luaScope, "value");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 9);
  }
}

TEST_F(ScriptingSystemTest, CallsScriptStartFunctionOnStart) {
  auto handle = scriptingSystem.addScript("scripting-system-tester.lua");
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope, nullptr);

  scriptingSystem.start();
  EXPECT_NE(component.scope, nullptr);

  auto *luaScope = static_cast<lua_State *>(component.scope);
  {
    lua_getglobal(luaScope, "value");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, -1);
  }
}

TEST_F(ScriptingSystemTest, CallsScriptingStartFunctionOnlyOnceOnStart) {
  auto handle = scriptingSystem.addScript("scripting-system-tester.lua");
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);
  EXPECT_EQ(component.scope, nullptr);

  // Call 10 times
  for (size_t i = 0; i < 10; ++i) {
    scriptingSystem.start();
  }
  EXPECT_NE(component.scope, nullptr);

  auto *luaScope = static_cast<lua_State *>(component.scope);
  {
    lua_getglobal(luaScope, "value");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, -1);
  }
}

TEST_F(ScriptingSystemTest, RegistersEventsOnStart) {
  auto handle = scriptingSystem.addScript("scripting-system-tester.lua");
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start();

  EXPECT_LT(component.onCollisionStart, liquid::EVENT_OBSERVER_MAX);
  EXPECT_LT(component.onCollisionEnd, liquid::EVENT_OBSERVER_MAX);
  EXPECT_LT(component.onKeyPress, liquid::EVENT_OBSERVER_MAX);
  EXPECT_LT(component.onKeyRelease, liquid::EVENT_OBSERVER_MAX);
}

TEST_F(ScriptingSystemTest,
       DoesNotCallScriptCollisionEventIfEntityDidNotCollide) {
  auto handle = scriptingSystem.addScript("scripting-system-tester.lua");
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start();

  eventSystem.dispatch(liquid::CollisionEvent::CollisionStarted, {5, 6});
  eventSystem.poll();

  auto *luaScope = static_cast<lua_State *>(component.scope);
  {
    lua_getglobal(luaScope, "event");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 0);
  }
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionStartEventIfEntityCollided) {
  auto handle = scriptingSystem.addScript("scripting-system-tester.lua");
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start();

  eventSystem.dispatch(liquid::CollisionEvent::CollisionStarted, {entity, 6});
  eventSystem.poll();

  auto *luaScope = static_cast<lua_State *>(component.scope);
  {
    lua_getglobal(luaScope, "event");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 1);
  }

  {
    lua_getglobal(luaScope, "target");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 6);
  }
}

TEST_F(ScriptingSystemTest, CallsScriptCollisionEndEventIfEntityCollided) {
  auto handle = scriptingSystem.addScript("scripting-system-tester.lua");
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start();

  eventSystem.dispatch(liquid::CollisionEvent::CollisionEnded, {5, entity});
  eventSystem.poll();

  auto *luaScope = static_cast<lua_State *>(component.scope);
  {
    lua_getglobal(luaScope, "event");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 2);
  }

  {
    lua_getglobal(luaScope, "target");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 5);
  }
}

TEST_F(ScriptingSystemTest, CallsScriptKeyPressEventIfKeyIsPressed) {
  auto handle = scriptingSystem.addScript("scripting-system-tester.lua");
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start();

  eventSystem.dispatch(liquid::KeyboardEvent::Pressed, {15});
  eventSystem.poll();

  auto *luaScope = static_cast<lua_State *>(component.scope);
  {
    lua_getglobal(luaScope, "event");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 3);
  }

  {
    lua_getglobal(luaScope, "key_value");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 15);
  }
}

TEST_F(ScriptingSystemTest, CallsScriptKeyReleaseEventIfKeyIsReleased) {
  auto handle = scriptingSystem.addScript("scripting-system-tester.lua");
  auto entity = entityContext.createEntity();
  entityContext.setComponent<liquid::ScriptingComponent>(entity, {handle});

  auto &component =
      entityContext.getComponent<liquid::ScriptingComponent>(entity);

  scriptingSystem.start();

  eventSystem.dispatch(liquid::KeyboardEvent::Released, {35});
  eventSystem.poll();

  auto *luaScope = static_cast<lua_State *>(component.scope);
  {
    lua_getglobal(luaScope, "event");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 4);
  }

  {
    lua_getglobal(luaScope, "key_value");
    int value = static_cast<int>(lua_tointeger(luaScope, -1));
    lua_pop(luaScope, -1);
    EXPECT_EQ(value, 35);
  }
}
