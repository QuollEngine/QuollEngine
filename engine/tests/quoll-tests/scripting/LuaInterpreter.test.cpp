#include "quoll/core/Base.h"
#include "quoll/scripting/LuaHeaders.h"
#include "quoll/scripting/LuaInterpreter.h"

#include "quoll-tests/Testing.h"

inline std::vector<uint8_t> readFileIntoBuffer(const quoll::Path &fileName) {
  std::ifstream file(fileName);

  LIQUID_ASSERT(file.good(), "File cannot be opened");

  std::ostringstream ss;
  ss << file.rdbuf();
  const std::string &s = ss.str();
  std::vector<uint8_t> bytes(s.begin(), s.end());
  file.close();

  return bytes;
}

class LuaInterpreterTest : public ::testing::Test {
public:
  quoll::LuaInterpreter interpreter;
};

TEST_F(LuaInterpreterTest, EvaluateScript) {
  auto scope = interpreter.createScope();

  auto buffer = readFileIntoBuffer(FixturesPath / "component-script.lua");

  interpreter.evaluate(buffer, scope);

  auto *luaScope = scope.getLuaState();

  lua_getglobal(luaScope, "state");
  EXPECT_TRUE(lua_isstring(luaScope, -1));
  EXPECT_EQ(quoll::String(lua_tostring(luaScope, -1)), "none");
  lua_pop(luaScope, -1);

  lua_getglobal(luaScope, "start");
  EXPECT_TRUE(lua_isfunction(luaScope, -1));
  EXPECT_EQ(lua_pcall(luaScope, 0, 1, 0), LUA_OK);
  lua_pop(luaScope, lua_gettop(luaScope));

  lua_getglobal(luaScope, "state");
  EXPECT_TRUE(lua_isstring(luaScope, -1));
  EXPECT_EQ(quoll::String(lua_tostring(luaScope, -1)), "start");
  lua_pop(luaScope, -1);

  lua_getglobal(luaScope, "update");
  EXPECT_TRUE(lua_isfunction(luaScope, -1));
  EXPECT_EQ(lua_pcall(luaScope, 0, 1, 0), LUA_OK);
  lua_pop(luaScope, lua_gettop(luaScope));

  lua_getglobal(luaScope, "state");
  EXPECT_TRUE(lua_isstring(luaScope, -1));
  EXPECT_EQ(quoll::String(lua_tostring(luaScope, -1)), "update");
  lua_pop(luaScope, -1);

  interpreter.destroyScope(scope);
}
