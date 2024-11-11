#include "quoll/core/Base.h"
#include "quoll/qui/native/Custom.h"
#include "quoll/qui/reactive/Value.h"
#include "MockView.h"
#include "QuiComponentTest.h"

class QuiCustomComponentTest : public QuiComponentTest {};

class Person : public qui::Component {
public:
  Person(qui::Value<quoll::String> name) : mName(name) { mView.value = 20; }

  void build(qui::BuildContext &context) override { mAge = 20; }

  constexpr quoll::String getName() const { return mName(); }

  constexpr uint32_t getAge() const { return mAge; }

  qui::View *getView() override { return &mView; }

private:
  qui::Value<quoll::String> mName;
  uint32_t mAge = 0;
  MockView mView;
};

TEST_F(QuiCustomComponentTest, ComponentWithoutPropsCreatesElementOnCall) {
  auto Test = qui::component([]() { return Person("John"); });
  auto tree = qui::Qui::createTree(Test());

  auto *component = dynamic_cast<const qui::Custom *>(tree.root.getComponent());
  auto *person =
      dynamic_cast<const Person *>(component->getResult().getComponent());

  EXPECT_EQ(person->getName(), "John");
  EXPECT_EQ(person->getAge(), 20);
}

TEST_F(QuiCustomComponentTest, ComponentWithPropsCreatesElementOnCall) {
  auto Test = qui::component(
      [](qui::Value<quoll::String> name) { return Person(name); });
  auto tree = qui::Qui::createTree(Test("John"));

  auto *component = dynamic_cast<const qui::Custom *>(tree.root.getComponent());
  auto *person =
      dynamic_cast<const Person *>(component->getResult().getComponent());

  EXPECT_EQ(person->getName(), "John");
  EXPECT_EQ(person->getAge(), 20);
}

TEST_F(QuiCustomComponentTest, GettingViewReturnsViewOfResult) {
  auto Test = qui::component([]() { return Person("John"); });
  auto tree = qui::Qui::createTree(Test());

  auto *view = dynamic_cast<MockView *>(tree.root.getView());

  ASSERT_NE(view, nullptr);
  EXPECT_EQ(view->value, 20);
}
