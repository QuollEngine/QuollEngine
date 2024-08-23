#include "quoll/core/Base.h"
#include "quoll/ui/UILuaTable.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using UILuaTableTest = LuaScriptingInterfaceTestBase;

TEST_F(UILuaTableTest, ImageReturnsTableForImageComponent) {
  auto entity = entityDatabase.entity();
  call(entity, "uiElementImage");
}

TEST_F(UILuaTableTest, TextReturnsTableForTextComponent) {
  auto entity = entityDatabase.entity();
  call(entity, "uiElementText");
}

TEST_F(UILuaTableTest, ViewReturnsTableForViewComponent) {
  auto entity = entityDatabase.entity();
  call(entity, "uiElementView");
}

class UIViewStylesLuaTableTest : public LuaScriptingInterfaceTestBase {
public:
  template <typename R, typename... T>
  R callWithArgs(quoll::Entity entity, const quoll::String &functionName,
                 T &&...args) {
    auto state = start(entity);

    return state[functionName](args...);
  }

  quoll::UIView defaultValue{};
};

TEST_F(UIViewStylesLuaTableTest, ReturnsViewWithProvidedDirection) {
  auto entity = entityDatabase.entity();
  auto c = [this, entity](quoll::String value) {
    return callWithArgs<quoll::UIView>(entity, "uiElementViewDirection", value)
        .style;
  };

  EXPECT_EQ(c("row").direction, YGFlexDirectionRow);
  EXPECT_EQ(c("row-reverse").direction, YGFlexDirectionRowReverse);
  EXPECT_EQ(c("column").direction, YGFlexDirectionColumn);
  EXPECT_EQ(c("column-reverse").direction, YGFlexDirectionColumnReverse);
  EXPECT_EQ(c("").direction, defaultValue.style.direction);
  EXPECT_EQ(c("test").direction, defaultValue.style.direction);
  EXPECT_EQ(
      callWithArgs<quoll::UIView>(entity, "uiElementViewDirection", sol::nil)
          .style.direction,
      defaultValue.style.direction);
  EXPECT_EQ(callWithArgs<quoll::UIView>(entity, "uiElementViewDirection", 15)
                .style.direction,
            defaultValue.style.direction);
}

TEST_F(UIViewStylesLuaTableTest, ReturnsViewWithProvidedAlignItems) {
  auto entity = entityDatabase.entity();
  auto c = [this, entity](quoll::String value) {
    return callWithArgs<quoll::UIView>(entity, "uiElementViewAlignItems", value)
        .style;
  };

  EXPECT_EQ(c("start").alignItems, YGAlignFlexStart);
  EXPECT_EQ(c("end").alignItems, YGAlignFlexEnd);
  EXPECT_EQ(c("center").alignItems, YGAlignCenter);
  EXPECT_EQ(c("baseline").alignItems, YGAlignBaseline);
  EXPECT_EQ(c("stretch").alignItems, YGAlignStretch);
  EXPECT_EQ(c("space-around").alignItems, YGAlignSpaceAround);
  EXPECT_EQ(c("space-between").alignItems, YGAlignSpaceBetween);
  EXPECT_EQ(c("").alignItems, defaultValue.style.alignItems);
  EXPECT_EQ(c("test").alignItems, defaultValue.style.alignItems);
  EXPECT_EQ(
      callWithArgs<quoll::UIView>(entity, "uiElementViewAlignItems", sol::nil)
          .style.alignItems,
      defaultValue.style.alignItems);
  EXPECT_EQ(callWithArgs<quoll::UIView>(entity, "uiElementViewAlignItems", 15)
                .style.alignItems,
            defaultValue.style.alignItems);
}

TEST_F(UIViewStylesLuaTableTest, ReturnsViewWithProvidedAlignContent) {
  auto entity = entityDatabase.entity();
  auto c = [this, entity](quoll::String value) {
    return callWithArgs<quoll::UIView>(entity, "uiElementViewAlignContent",
                                       value)
        .style;
  };

  EXPECT_EQ(c("start").alignContent, YGAlignFlexStart);
  EXPECT_EQ(c("end").alignContent, YGAlignFlexEnd);
  EXPECT_EQ(c("center").alignContent, YGAlignCenter);
  EXPECT_EQ(c("baseline").alignContent, YGAlignBaseline);
  EXPECT_EQ(c("stretch").alignContent, YGAlignStretch);
  EXPECT_EQ(c("space-around").alignContent, YGAlignSpaceAround);
  EXPECT_EQ(c("space-between").alignContent, YGAlignSpaceBetween);
  EXPECT_EQ(c("").alignContent, defaultValue.style.alignContent);
  EXPECT_EQ(c("test").alignContent, defaultValue.style.alignContent);
  EXPECT_EQ(
      callWithArgs<quoll::UIView>(entity, "uiElementViewAlignContent", sol::nil)
          .style.alignContent,
      defaultValue.style.alignContent);
  EXPECT_EQ(callWithArgs<quoll::UIView>(entity, "uiElementViewAlignContent", 15)
                .style.alignContent,
            defaultValue.style.alignContent);
}

TEST_F(UIViewStylesLuaTableTest, ReturnsViewWithProvidedJustifyContent) {
  auto entity = entityDatabase.entity();
  auto c = [this, entity](quoll::String value) {
    return callWithArgs<quoll::UIView>(entity, "uiElementViewJustifyContent",
                                       value)
        .style;
  };

  EXPECT_EQ(c("start").justifyContent, YGJustifyFlexStart);
  EXPECT_EQ(c("end").justifyContent, YGJustifyFlexEnd);
  EXPECT_EQ(c("center").justifyContent, YGJustifyCenter);
  EXPECT_EQ(c("space-around").justifyContent, YGJustifySpaceAround);
  EXPECT_EQ(c("space-between").justifyContent, YGJustifySpaceBetween);
  EXPECT_EQ(c("space-evenly").justifyContent, YGJustifySpaceEvenly);
  EXPECT_EQ(c("").justifyContent, defaultValue.style.justifyContent);
  EXPECT_EQ(c("test").justifyContent, defaultValue.style.justifyContent);
  EXPECT_EQ(callWithArgs<quoll::UIView>(entity, "uiElementViewJustifyContent",
                                        sol::nil)
                .style.justifyContent,
            defaultValue.style.justifyContent);
  EXPECT_EQ(
      callWithArgs<quoll::UIView>(entity, "uiElementViewJustifyContent", 15)
          .style.justifyContent,
      defaultValue.style.justifyContent);
}

TEST_F(UIViewStylesLuaTableTest, ReturnsViewWithProvidedFlexGrow) {
  auto entity = entityDatabase.entity();
  auto c = [this, entity](float value) {
    return callWithArgs<quoll::UIView>(entity, "uiElementViewFlexGrow", value)
        .style;
  };

  EXPECT_EQ(c(0.0f).grow, 0.0f);
  EXPECT_EQ(c(1.0f).grow, 1.0f);
  EXPECT_EQ(c(2.0f).grow, 2.0f);
  EXPECT_EQ(c(2.5f).grow, 2.5f);
  EXPECT_EQ(c(-1.0f).grow, 0.0f);
  EXPECT_EQ(
      callWithArgs<quoll::UIView>(entity, "uiElementViewFlexGrow", sol::nil)
          .style.grow,
      defaultValue.style.grow);
  EXPECT_EQ(callWithArgs<quoll::UIView>(entity, "uiElementViewFlexGrow", "test")
                .style.grow,
            defaultValue.style.grow);
}

TEST_F(UIViewStylesLuaTableTest, ReturnsViewWithProvidedFlexShrink) {
  auto entity = entityDatabase.entity();
  auto c = [this, entity](float value) {
    return callWithArgs<quoll::UIView>(entity, "uiElementViewFlexShrink", value)
        .style;
  };

  EXPECT_EQ(c(0.0f).shrink, 0.0f);
  EXPECT_EQ(c(1.0f).shrink, 1.0f);
  EXPECT_EQ(c(2.0f).shrink, 2.0f);
  EXPECT_EQ(c(2.5f).shrink, 2.5f);
  EXPECT_EQ(c(-1.0f).shrink, 0.0f);
  EXPECT_EQ(
      callWithArgs<quoll::UIView>(entity, "uiElementViewFlexShrink", sol::nil)
          .style.shrink,
      defaultValue.style.shrink);
  EXPECT_EQ(
      callWithArgs<quoll::UIView>(entity, "uiElementViewFlexShrink", "test")
          .style.shrink,
      defaultValue.style.shrink);
}

TEST_F(UIViewStylesLuaTableTest, ReturnsViewWithProvidedBackgroundColor) {
  auto entity = entityDatabase.entity();
  auto c = [this, entity](std::array<f32, 4> value) {
    return callWithArgs<quoll::UIView>(entity, "uiElementViewBackgroundColor",
                                       value)
        .style;
  };

  EXPECT_EQ(c({0.0f, 0.0f, 0.0f, 0.0f}).backgroundColor,
            glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(c({1.0f, 1.0f, 1.0f, 1.0f}).backgroundColor,
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  EXPECT_EQ(c({0.2f, 0.3f, 0.4f, 0.5f}).backgroundColor,
            glm::vec4(0.2f, 0.3f, 0.4f, 0.5f));
  EXPECT_EQ(c({-1.0f, -1.0f, -1.0f, -1.0f}).backgroundColor,
            glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
  EXPECT_EQ(c({2.0f, 2.0f, 2.0f, 2.0f}).backgroundColor,
            glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  EXPECT_EQ(c({-1.0f, 0.5f, 2.0f, 0.2f}).backgroundColor,
            glm::vec4(0.0f, 0.5f, 1.0f, 0.2f));

  EXPECT_EQ(callWithArgs<quoll::UIView>(entity, "uiElementViewBackgroundColor",
                                        sol::nil)
                .style.backgroundColor,
            defaultValue.style.backgroundColor);
  EXPECT_EQ(callWithArgs<quoll::UIView>(entity, "uiElementViewBackgroundColor",
                                        "test")
                .style.backgroundColor,
            defaultValue.style.backgroundColor);
  EXPECT_EQ(
      callWithArgs<quoll::UIView>(entity, "uiElementViewBackgroundColor", 12)
          .style.backgroundColor,
      defaultValue.style.backgroundColor);
}
