#include "quoll/core/Base.h"
#include "ReactiveTestBase.h"

class QuiComputationTest : public ReactiveTestBase {};

TEST_F(QuiComputationTest, CreatesComputationFromSignal) {
  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() + 20; });

  EXPECT_EQ(c1(), 25);
}

TEST_F(QuiComputationTest,
       UpdatingSignalUpdatesComputationAndNotifiesObservers) {
  ObserverFn o1, o2;

  EXPECT_CALL(o1, Call()).Times(1);
  EXPECT_CALL(o2, Call()).Times(1);

  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() + 20; });

  c1.observe(o1.AsStdFunction());
  c1.observe(o2.AsStdFunction());

  s1.set(20);

  EXPECT_EQ(c1(), 40);
}

TEST_F(QuiComputationTest, CreatesComputationFromOtherComputation) {
  auto s1 = scope.signal(5);

  auto c1 = scope.computation([s1] { return s1() + 20; });
  auto c2 = scope.computation([c1] { return c1() * 2; });

  EXPECT_EQ(c2(), 50);
}

TEST_F(
    QuiComputationTest,
    UpdatingSignalOfParentComputationUpdatesChildComputationAndNotifiesObservers) {
  ObserverFn o1, o2;

  EXPECT_CALL(o1, Call()).Times(1);
  EXPECT_CALL(o2, Call()).Times(1);

  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() + 20; });
  auto c2 = scope.computation([c1] { return c1() * 2; });

  c2.observe(o1.AsStdFunction());
  c2.observe(o2.AsStdFunction());

  s1.set(20);

  EXPECT_EQ(c2(), 80);
}

TEST_F(QuiComputationTest, CreatesComputationFromSignalAndComputation) {
  auto s1 = scope.signal(5);
  auto s2 = scope.signal(10);

  auto c1 = scope.computation([s1] { return s1() + 20; });
  auto c2 = scope.computation([c1, s2] { return c1() * s2(); });

  EXPECT_EQ(c2(), 250);
}

TEST_F(
    QuiComputationTest,
    EachUpdateToParentOfComputationUpdatesChildComputationAndNotifiesObservers) {
  ObserverFn o1, o2;

  EXPECT_CALL(o1, Call()).Times(2);
  EXPECT_CALL(o2, Call()).Times(2);

  auto s1 = scope.signal(5);
  auto s2 = scope.signal(10);

  auto c1 = scope.computation([s1] { return s1() + 20; });
  auto c2 = scope.computation([c1, s2] { return c1() * s2(); });

  c2.observe(o1.AsStdFunction());
  c2.observe(o2.AsStdFunction());

  s1.set(20);
  s2.set(100);

  EXPECT_EQ(c2(), 4000);
}

/**
 *    S1
 *  /   \
 * C1	C2
 *  \   /
 *    C3
 */
TEST_F(
    QuiComputationTest,
    WhenComputationDependsOnTwoComputationsThatDependOnTheSameSignalUpdatingSignalNotifiesObserversTwice) {
  ObserverFn o1, o2;

  EXPECT_CALL(o1, Call()).Times(2);
  EXPECT_CALL(o2, Call()).Times(2);

  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() + 20; });
  auto c2 = scope.computation([s1] { return s1() + 40; });
  auto c3 = scope.computation([c1, c2] { return c1() * c2(); });

  c3.observe(o1.AsStdFunction());
  c3.observe(o2.AsStdFunction());

  s1.set(10);

  EXPECT_EQ(c3(), 1500);
}

using QuiComputationFormatTest = QuiComputationTest;

TEST_F(QuiComputationFormatTest, FormatsSignalValue) {
  auto s1 = scope.signal(5);
  auto output = scope.format("{}", s1);
  EXPECT_EQ(output(), "5");
}

TEST_F(QuiComputationFormatTest, ReformatsWhenSignalIsUpdated) {
  auto s1 = scope.signal(5);
  auto output = scope.format("{}", s1);

  s1.set(10);
  EXPECT_EQ(output(), "10");
}

TEST_F(QuiComputationFormatTest, FormatsComputationValue) {
  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() + 20; });

  auto output = scope.format("{}", c1);
  EXPECT_EQ(output(), "25");
}

TEST_F(QuiComputationFormatTest, ReformatsWhenComputationValueIsUpdated) {
  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() + 20; });

  auto output = scope.format("{}", c1);

  s1.set(20);
  EXPECT_EQ(output(), "40");
}

TEST_F(QuiComputationFormatTest, FormatsSignalComputationAndStaticArgs) {
  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() + 20; });

  auto output = scope.format("{} + {} = {}", s1, 20, c1);
  EXPECT_EQ(output(), "5 + 20 = 25");
}

TEST_F(QuiComputationFormatTest,
       ReformatsSignalComputationAndStaticWhenSignalIsUpdated) {
  auto s1 = scope.signal(5);
  auto c1 = scope.computation([s1] { return s1() + 20; });

  auto output = scope.format("{} + {} = {}", s1, 20, c1);

  s1.set(20);
  EXPECT_EQ(output(), "20 + 20 = 40");
}
