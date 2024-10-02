#include "quoll/core/Base.h"
#include "quoll/qui/component/Element.h"
#include "MockComponent.h"
#include "QuiComponentTest.h"

class QuiElementTest : public QuiComponentTest {
public:
};

TEST_F(QuiElementTest, CreatesElementFromComponent) {
  qui::Element element = MockComponent(20);
  auto *component = dynamic_cast<const MockComponent *>(element.getComponent());

  EXPECT_EQ(component->value, 20);
  EXPECT_EQ(dynamic_cast<MockView *>(element.getView())->value, 0);
  EXPECT_EQ(component->buildCount, 0);
}

TEST_F(QuiElementTest, BuildingElementBuildsComponent) {
  qui::Element element = MockComponent(20);
  auto *component = dynamic_cast<const MockComponent *>(element.getComponent());

  element.build(buildContext);

  EXPECT_EQ(component->value, 20);
  EXPECT_EQ(dynamic_cast<MockView *>(element.getView())->value, 20);
  EXPECT_EQ(component->buildCount, 1);
}

TEST_F(QuiElementTest, RebuildingElementDoesNotBuildComponentAgain) {
  qui::Element element = MockComponent(20);
  auto *component = dynamic_cast<const MockComponent *>(element.getComponent());

  element.build(buildContext);
  element.build(buildContext);
  element.build(buildContext);

  EXPECT_EQ(component->value, 20);
  EXPECT_EQ(dynamic_cast<MockView *>(element.getView())->value, 20);
  EXPECT_EQ(component->buildCount, 1);
}
