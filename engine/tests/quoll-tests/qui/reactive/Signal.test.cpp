#include "quoll/core/Base.h"
#include "ReactiveTestBase.h"

class QuiSignalTest : public ReactiveTestBase {};

TEST_F(QuiSignalTest, CreatesViewFromSignal) {
  auto signal = scope.signal(5);

  EXPECT_EQ(signal(), 5);
}

TEST_F(QuiSignalTest, UpdateForwardsItToSignal) {
  ObserverFn o1, o2;

  EXPECT_CALL(o1, Call()).Times(1);
  EXPECT_CALL(o2, Call()).Times(1);

  auto signal = scope.signal(5);

  signal.observe(o1.AsStdFunction());
  signal.observe(o2.AsStdFunction());

  signal.set(10);
}

TEST_F(QuiSignalTest, RemoveObserverRemovesItFromSignal) {
  ObserverFn o1, o2;

  EXPECT_CALL(o1, Call()).Times(0);
  EXPECT_CALL(o2, Call()).Times(0);

  auto signal = scope.signal(5);

  auto r1 = signal.observe(o1.AsStdFunction());
  auto r2 = signal.observe(o2.AsStdFunction());

  r1.unobserve();
  r2.unobserve();

  signal.set(10);
}

TEST_F(QuiSignalTest, UpdatingSignalWithTheSameValueDoesNotNotifyObservers) {
  ObserverFn o1, o2;

  EXPECT_CALL(o1, Call()).Times(0);
  EXPECT_CALL(o2, Call()).Times(0);

  auto signal = scope.signal(5);

  signal.observe(o1.AsStdFunction());
  signal.observe(o2.AsStdFunction());

  signal.set(5);
}

TEST_F(QuiSignalTest, CreatesVectorSignal) {
  auto signal = scope.signal(std::vector{1, 2, 3});

  EXPECT_EQ(signal().size(), 3);
  EXPECT_EQ(signal().at(0), 1);
  EXPECT_EQ(signal().at(1), 2);
  EXPECT_EQ(signal().at(2), 3);
}

TEST_F(QuiSignalTest, CreatesVectorSignalFromInitializerList) {
  auto signal = scope.signal({1, 2, 3});

  EXPECT_EQ(signal().size(), 3);
  EXPECT_EQ(signal().at(0), 1);
  EXPECT_EQ(signal().at(1), 2);
  EXPECT_EQ(signal().at(2), 3);
}

TEST_F(QuiSignalTest, PushingItemToVectorSignalNotifiesObservers) {
  ObserverFn o1, o2;

  EXPECT_CALL(o1, Call()).Times(1);
  EXPECT_CALL(o2, Call()).Times(1);

  auto signal = scope.signal(std::vector{1, 2, 3});
  signal.observe(o1.AsStdFunction());
  signal.observe(o2.AsStdFunction());

  signal.push_back(5);

  EXPECT_EQ(signal().size(), 4);
  EXPECT_EQ(signal().at(3), 5);
}

TEST_F(QuiSignalTest, CreatesStringSignalWhenConstCharIsPassed) {
  auto signal = scope.signal("Hello world");

  // String should have length() method
  EXPECT_EQ(signal().length(), 11);
}
