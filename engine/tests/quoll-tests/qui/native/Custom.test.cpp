#include "quoll/core/Base.h"
#include "quoll/qui/native/Custom.h"
#include "quoll/qui/reactive/Value.h"
#include "quoll-tests/Testing.h"

class QuiCustomComponentTest : public ::testing::Test {};

class PersonView : public qui::View {
public:
  int data = 20;

  qui::LayoutOutput layout(const qui::LayoutInput &input) override {
    return qui::LayoutOutput{};
  }
};

class Person : public qui::Component {
public:
  Person(qui::Value<quoll::String> name) : mName(name) {}

  void build() override { mAge = 20; }

  constexpr quoll::String getName() const { return mName(); }

  constexpr uint32_t getAge() const { return mAge; }

  qui::View *getView() override { return &mPersonView; }

private:
  qui::Value<quoll::String> mName;
  uint32_t mAge = 0;
  PersonView mPersonView;
};

TEST_F(QuiCustomComponentTest, ComponentWithoutPropsCreatesElementOnCall) {
  auto Test = qui::component([]() { return Person("John"); });

  auto element = Test();

  auto *component = dynamic_cast<qui::Custom *>(element.getComponent());
  auto *person = dynamic_cast<Person *>(component->getResult().getComponent());

  EXPECT_EQ(person->getName(), "John");
  EXPECT_EQ(person->getAge(), 0);
}

TEST_F(QuiCustomComponentTest, ComponentWithPropsCreatesElementOnCall) {
  auto Test = qui::component(
      [](qui::Value<quoll::String> name) { return Person(name); });

  auto element = Test("John");

  auto *component = dynamic_cast<qui::Custom *>(element.getComponent());
  auto *person = dynamic_cast<Person *>(component->getResult().getComponent());

  EXPECT_EQ(person->getName(), "John");
  EXPECT_EQ(person->getAge(), 0);
}

TEST_F(QuiCustomComponentTest, ComponentWithScopeCreatesElementOnCall) {
  auto Test = qui::component([](qui::Scope &scope) {
    auto name = scope.signal("John");
    return Person(name);
  });

  auto element = Test();

  auto *component = dynamic_cast<qui::Custom *>(element.getComponent());
  auto *person = dynamic_cast<Person *>(component->getResult().getComponent());

  EXPECT_EQ(person->getName(), "John");
  EXPECT_EQ(person->getAge(), 0);
}

TEST_F(QuiCustomComponentTest, ComponentWithScopeAndPropsCreatesElementOnCall) {
  auto Test =
      qui::component([](qui::Scope &scope, qui::Value<quoll::String> name) {
        auto fullName = scope.computation([name] { return name() + " Doe"; });

        return Person(fullName);
      });

  auto element = Test("John");

  auto *component = dynamic_cast<qui::Custom *>(element.getComponent());
  auto *person = dynamic_cast<Person *>(component->getResult().getComponent());

  EXPECT_EQ(person->getName(), "John Doe");
  EXPECT_EQ(person->getAge(), 0);
}

TEST_F(QuiCustomComponentTest, BuildingComponnetBuildsTheResult) {
  auto Test = qui::component([]() { return Person("John"); });

  auto element = Test();

  element.getComponent()->build();

  auto *component = dynamic_cast<qui::Custom *>(element.getComponent());
  auto *person = dynamic_cast<Person *>(component->getResult().getComponent());

  EXPECT_EQ(person->getName(), "John");
  EXPECT_EQ(person->getAge(), 20);
}

TEST_F(QuiCustomComponentTest, GettingViewReturnsViewOfResult) {
  auto Test = qui::component([]() { return Person("John"); });

  auto element = Test();

  auto *component = element.getComponent();
  auto *view = dynamic_cast<PersonView *>(component->getView());

  ASSERT_NE(view, nullptr);
  EXPECT_EQ(view->data, 20);
}
