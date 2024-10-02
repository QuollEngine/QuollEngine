#include "quoll/core/Base.h"
#include "quoll/qui/component/Element.h"
#include "quoll/qui/native/Text.h"
#include "quoll/qui/reactive/Scope.h"
#include "QuiComponentTest.h"

class QuiTextTest : public QuiComponentTest {};

TEST_F(QuiTextTest, CreatesText) {
  qui::Element el = qui::Text("Hello world");
  auto *text = static_cast<const qui::Text *>(el.getComponent());

  EXPECT_EQ(text->getText(), "Hello world");
  EXPECT_EQ(text->getColor(), qui::Color::Black);
}

TEST_F(QuiTextTest, CreatesTextWithAllProps) {
  qui::Element el = qui::Text("Hello world").color(qui::Color::Red);
  auto *text = static_cast<const qui::Text *>(el.getComponent());

  EXPECT_EQ(text->getText(), "Hello world");
  EXPECT_EQ(text->getColor(), qui::Color::Red);
}

TEST_F(QuiTextTest, BuildingTextUpdatesView) {
  qui::Element el = qui::Text("Hello world").color(qui::Color::Red);

  el.build(buildContext);

  auto *view = static_cast<qui::TextView *>(el.getView());

  EXPECT_EQ(view->getText(), "Hello world");
  EXPECT_EQ(view->getColor(), qui::Color::Red);
}

TEST_F(QuiTextTest, UpdatingTextPropertiesAfterBuildUpdatesTheView) {
  qui::Scope scope;
  auto text = scope.signal("Hello world");
  auto color = scope.signal(qui::Color::Red);

  qui::Element el = qui::Text(text).color(color);
  el.build(buildContext);
  auto *view = static_cast<qui::TextView *>(el.getView());

  EXPECT_EQ(view->getText(), "Hello world");
  EXPECT_EQ(view->getColor(), qui::Color::Red);

  text.set("Testing");
  color.set(qui::Color::Green);

  EXPECT_EQ(view->getText(), "Testing");
  EXPECT_EQ(view->getColor(), qui::Color::Green);
}
