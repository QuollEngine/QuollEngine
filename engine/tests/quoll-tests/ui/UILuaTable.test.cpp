#include "quoll/core/Base.h"
#include "quoll/ui/UILuaTable.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using UILuaTableTest = LuaScriptingInterfaceTestBase;

TEST_F(UILuaTableTest, ImageReturnsTableForImageComponent) {
  auto entity = entityDatabase.create();
  call(entity, "ui_element_image");
}

TEST_F(UILuaTableTest, TextReturnsTableForTextComponent) {
  auto entity = entityDatabase.create();
  call(entity, "ui_element_text");
}

TEST_F(UILuaTableTest, ViewReturnsTableForTextComponent) {
  auto entity = entityDatabase.create();
  call(entity, "ui_element_view");
}
