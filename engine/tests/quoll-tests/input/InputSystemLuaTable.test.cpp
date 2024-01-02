#include "quoll/core/Base.h"
#include "quoll/core/Name.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class InputSystemLuaTableTest : public LuaScriptingInterfaceTestBase {
public:
  InputSystemLuaTableTest()
      : LuaScriptingInterfaceTestBase("input-system-service.lua") {}
};

TEST_F(InputSystemLuaTableTest, RegistersKeyPressEvent) {
  auto entity = entityDatabase.create();
  auto state = start(entity);

  windowSignals.onKeyPress().notify(
      quoll::KeyboardEvent{.key = 10, .scanCode = 20, .mods = 3});

  EXPECT_TRUE(state["event"].is<quoll::String>());
  EXPECT_TRUE(state["key"].is<u32>());
  EXPECT_TRUE(state["mods"].is<u32>());

  EXPECT_EQ(state["event"].get<quoll::String>(), "press");
  EXPECT_EQ(state["key"].get<u32>(), 10);
  EXPECT_EQ(state["mods"].get<u32>(), 3);
}

TEST_F(InputSystemLuaTableTest, RegistersKeyReleaseEvent) {
  auto entity = entityDatabase.create();
  auto state = start(entity);

  windowSignals.onKeyRelease().notify(
      quoll::KeyboardEvent{.key = 10, .scanCode = 20, .mods = 3});

  EXPECT_TRUE(state["event"].is<quoll::String>());
  EXPECT_TRUE(state["key"].is<u32>());
  EXPECT_TRUE(state["mods"].is<u32>());

  EXPECT_EQ(state["event"].get<quoll::String>(), "release");
  EXPECT_EQ(state["key"].get<u32>(), 10);
  EXPECT_EQ(state["mods"].get<u32>(), 3);
}
