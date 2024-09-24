#include "quoll/core/Base.h"
#include "quoll/qui/reactive/Value.h"
#include "ReactiveTestBase.h"

class QuiValueTest : public ReactiveTestBase {};

TEST_F(QuiValueTest, CreatesValueWithStaticData) {
  qui::Value<int> value(5);
  EXPECT_EQ(value(), 5);
}

TEST_F(QuiValueTest, CreatesValueWithSignalData) {
  auto s1 = scope.signal(5);
  qui::Value<int> value(s1);

  EXPECT_EQ(value(), 5);
}

TEST_F(QuiValueTest, UpdatingSignalUpdatesTheValue) {
  auto s1 = scope.signal(5);
  qui::Value<int> value(s1);

  s1.set(20);

  EXPECT_EQ(value(), 20);
}

TEST_F(QuiValueTest, CreatesValueWithComputationData) {
  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() * 2; });
  qui::Value<int> value(c1);

  EXPECT_EQ(value(), 10);
}

TEST_F(QuiValueTest, UpdatingSignalUpdatesTheValueWithComputation) {
  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() * 2; });
  qui::Value<int> value(c1);

  s1.set(20);

  EXPECT_EQ(value(), 40);
}

TEST_F(QuiValueTest, ValueUpdatesComputationWhenUsedWithinComputation) {
  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() * 2; });
  qui::Value<int> v1(c1);

  auto c2 = scope.computation([v1] { return v1() * 2; });
  qui::Value<int> v2(c2);

  s1.set(20);

  EXPECT_EQ(v1(), 40);
  EXPECT_EQ(v2(), 80);
}

TEST_F(QuiValueTest, UpdatingValueDataNotifiesValueObservers) {
  ObserverFn fn1, fn2;

  EXPECT_CALL(fn1, Call()).Times(1);
  EXPECT_CALL(fn2, Call()).Times(1);

  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() * 2; });
  qui::Value<int> v1(c1);
  auto c2 = scope.computation([v1] { return v1() * 2; });

  qui::Value<int> v2(c2);
  v2.observe(fn1.AsStdFunction());
  v2.observe(fn2.AsStdFunction());

  s1.set(20);
}

TEST_F(QuiValueTest, CopyingOrMovingNewStaticValueReplacesData) {
  // Copy static to static
  qui::Value<int> v1(5);
  EXPECT_EQ(v1(), 5);

  qui::Value<int> v2(10);
  v1 = v2;
  EXPECT_EQ(v1(), 10);

  // Move static to static
  v1 = qui::Value(15);
  EXPECT_EQ(v1(), 15);

  // Copy static to reactive
  auto s1 = scope.signal(20);
  qui::Value<int> v3(s1);
  EXPECT_EQ(v3(), 20);

  v3 = qui::Value(25);
  s1.set(30);
  EXPECT_EQ(v3(), 25);

  // Move static to reactive
  v3 = s1;
  v3 = qui::Value(35);
  s1.set(40);
  EXPECT_EQ(v3(), 35);
}

TEST_F(QuiValueTest, CopyingOrMovingNewReactiveValueReplacesData) {
  // Copy reactive to static
  qui::Value<int> v1(5);
  EXPECT_EQ(v1(), 5);

  auto s1 = scope.signal(10);
  qui::Value<int> v2(s1);
  v1 = v2;
  EXPECT_EQ(v1(), 10);
  s1.set(20);
  EXPECT_EQ(v1(), 20);

  // Move reactive to static
  v1 = qui::Value(15);
  v1 = s1;
  EXPECT_EQ(v1(), 20);
  s1.set(25);
  EXPECT_EQ(v1(), 25);

  // Copy reactive to reactive
  auto s2 = scope.signal(30);
  qui::Value<int> v3(s2);
  EXPECT_EQ(v3(), 30);

  v3 = v1;
  s2.set(25);
  EXPECT_EQ(v3(), 25);

  // Move reactive to reactive
  auto s3 = scope.signal(35);
  v3 = qui::Value(s3);
  EXPECT_EQ(v3(), 35);
  s3.set(40);
  EXPECT_EQ(v3(), 40);
}

TEST_F(QuiValueTest, CopyingOrMovingAnotherValueToValueClearsObservers) {
  ObserverFn fn1, fn2;

  EXPECT_CALL(fn1, Call()).Times(0);
  EXPECT_CALL(fn2, Call()).Times(0);

  auto s1 = scope.signal(5);
  qui::Value<int> v1(s1);
  v1.observe(fn1.AsStdFunction());
  v1.observe(fn2.AsStdFunction());

  auto s2 = scope.signal(10);
  v1 = qui::Value<int>(s2);

  s1.set(20);
  s2.set(20);

  auto s3 = scope.signal(20);

  qui::Value v2(s3);

  v1 = v2;

  s1.set(30);
  s2.set(30);
  s3.set(30);
}
