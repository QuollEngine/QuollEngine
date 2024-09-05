#include "quoll/core/Base.h"
#include "quoll/threads/ThreadPool.h"
#include "quoll-tests/Testing.h"

class ThreadPoolTest : public ::testing::Test {};

TEST_F(ThreadPoolTest, ExecutesTasksInDifferentThreads) {
  quoll::ThreadPool pool(2);

  auto r1 = pool.enqueue([](i32 a, i32 b) { return a + b; }, 10, 20);
  auto r2 = pool.enqueue([](i32 a, i32 b) { return a + b; }, 30, 40);
  auto r3 = pool.enqueue([](i32 a, i32 b) { return a + b; }, 50, 60);
  auto r4 = pool.enqueue([](i32 a, i32 b) { return a + b; }, 70, 80);

  r1.wait();
  r2.wait();
  r3.wait();
  r4.wait();

  EXPECT_EQ(r1.get(), 30);
  EXPECT_EQ(r2.get(), 70);
  EXPECT_EQ(r3.get(), 110);
  EXPECT_EQ(r4.get(), 150);
}

TEST_F(ThreadPoolTest,
       ReturnsErrorInFutureIfPoolIsDestroyedWhileTaskIsInQueue) {
  using namespace std::chrono_literals;

  quoll::ThreadPool pool(1);
  pool.enqueue(
      [](i32 a, i32 b) {
        std::this_thread::sleep_for(2000ms);
        return a + b;
      },
      10, 20);

  pool.~ThreadPool();

  auto task = pool.enqueue(
      [](i32 a, i32 b) {
        std::this_thread::sleep_for(2000ms);
        return a + b;
      },
      10, 20);

  task.wait();
  auto res = task.get();
  EXPECT_EQ(res.error().message(), "Thread pool is stopped");
}
