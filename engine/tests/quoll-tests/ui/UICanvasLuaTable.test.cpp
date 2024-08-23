#include "quoll/core/Base.h"
#include "quoll/ui/UICanvas.h"
#include "quoll/ui/UICanvasRenderRequest.h"
#include "quoll/ui/UILuaTable.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using UICanvasLuaTableTest = LuaScriptingInterfaceTestBase;

TEST_F(UICanvasLuaTableTest, DoesNothingIfNoUICanvasComponent) {
  auto entity = entityDatabase.entity();
  call(entity, "uiCanvasRender");
  EXPECT_FALSE(entity.has<quoll::UICanvasRenderRequest>());
}

TEST_F(UICanvasLuaTableTest, CreatesCanvasUIRenderRequestOnRender) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::UICanvas>({});
  call(entity, "uiCanvasRender");
  EXPECT_TRUE(entity.has<quoll::UICanvasRenderRequest>());

  auto view = entity.get_ref<quoll::UICanvasRenderRequest>()->view;

  EXPECT_EQ(view.children.size(), 6);
  EXPECT_EQ(std::get<quoll::UIImage>(view.children.at(0)).texture,
            quoll::TextureAssetHandle{10});
  EXPECT_EQ(std::get<quoll::UIText>(view.children.at(1)).content, "Hello");
  EXPECT_EQ(std::get<quoll::UIImage>(view.children.at(2)).texture,
            quoll::TextureAssetHandle{20});
  EXPECT_EQ(std::get<quoll::UIText>(view.children.at(3)).content, "Test");

  auto child = std::get<quoll::UIView>(view.children.at(4));
  EXPECT_EQ(child.children.size(), 2);
  EXPECT_EQ(std::get<quoll::UIText>(child.children.at(0)).content, "Child");
  EXPECT_EQ(std::get<quoll::UIImage>(child.children.at(1)).texture,
            quoll::TextureAssetHandle{30});
  EXPECT_EQ(std::get<quoll::UIView>(view.children.at(5)).children.size(), 0);
}
