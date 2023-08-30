#include "liquid/core/Base.h"
#include "liquid/events/EventPool.h"

#include "liquid-tests/Testing.h"

enum class TestEvent { ValueAdded, ValueRemoved };

struct TestObject {
  int value = 0;

  bool operator==(const TestObject &rhs) const { return value == rhs.value; }
};

class EventPoolTest : public ::testing::Test {
public:
  quoll::EventPool<TestEvent, TestObject> eventPool;

  MOCK_METHOD(void, onValueAdded, (const TestObject &));
  MOCK_METHOD(void, onValueRemoved, (const TestObject &));
};

TEST_F(EventPoolTest, DispatchingAddsEventToTheQueue) {
  eventPool.dispatch(TestEvent::ValueAdded, {2});
  EXPECT_EQ(eventPool.getQueue().front().data.value, 2);
}

TEST_F(EventPoolTest, AddsObserver) {
  EXPECT_CALL((*this), onValueAdded(TestObject{2})).Times(1);

  eventPool.observe(TestEvent::ValueAdded,
                    [this](const TestObject &data) { onValueAdded(data); });

  const auto &observers = eventPool.getObservers(TestEvent::ValueAdded);
  EXPECT_EQ(observers.size(), 1);
  observers.at(0)({2});
}

TEST_F(EventPoolTest, RemovesObserver) {
  auto id = eventPool.observe(TestEvent::ValueAdded,
                              [this](const TestObject &data) {});

  const auto &observers = eventPool.getObservers(TestEvent::ValueAdded);
  EXPECT_EQ(observers.size(), 1);

  eventPool.removeObserver(TestEvent::ValueAdded, id);
  EXPECT_EQ(observers.size(), 0);
}

TEST_F(EventPoolTest, PollsEventsAndCallsObservers) {
  EXPECT_CALL((*this), onValueAdded(TestObject{2})).Times(2);
  EXPECT_CALL((*this), onValueAdded(TestObject{4})).Times(2);
  EXPECT_CALL((*this), onValueAdded(TestObject{3})).Times(1);
  EXPECT_CALL((*this), onValueAdded(TestObject{6})).Times(1);
  EXPECT_CALL((*this), onValueRemoved(TestObject{2})).Times(3);

  eventPool.observe(TestEvent::ValueAdded,
                    [this](const TestObject &data) { onValueAdded(data); });
  eventPool.observe(TestEvent::ValueAdded, [this](const TestObject &data) {
    onValueAdded({data.value * 2});
  });
  eventPool.observe(TestEvent::ValueRemoved,
                    [this](const TestObject &data) { onValueRemoved(data); });

  eventPool.dispatch(TestEvent::ValueAdded, {2});
  eventPool.dispatch(TestEvent::ValueAdded, {3});
  eventPool.dispatch(TestEvent::ValueAdded, {2});
  eventPool.dispatch(TestEvent::ValueRemoved, {2});
  eventPool.dispatch(TestEvent::ValueRemoved, {2});
  eventPool.dispatch(TestEvent::ValueRemoved, {2});

  EXPECT_EQ(eventPool.getQueue().size(), 6);
  eventPool.poll();
  EXPECT_EQ(eventPool.getQueue().size(), 0);
}