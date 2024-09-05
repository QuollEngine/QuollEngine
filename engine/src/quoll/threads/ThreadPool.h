#pragma once

#include "quoll/asset/Result.h"

namespace quoll {

class ThreadPool {
public:
  using TaskFn = std::function<void()>;

public:
  ThreadPool(u32 numThreads);
  ~ThreadPool();

  template <typename Fn, typename... Args>
  std::future<Result<typename std::invoke_result_t<Fn, Args...>>>
  enqueue(Fn &&fn, Args &&...args) {
    QuollAssert(mWorkers.size() > 0, "Thread pool is not initialized");
    using ReturnType = Result<std::invoke_result_t<Fn, Args...>>;

    auto promise = std::make_shared<std::promise<ReturnType>>();
    auto result = promise->get_future();
    {
      std::unique_lock<std::mutex> lock(mTasksQueueMutex);

      if (mTerminate.load()) {
        promise->set_value(Error("Thread pool is stopped"));
        return result;
      }

      auto func = std::forward<Fn>(fn);
      auto boundArgs = std::make_tuple(std::forward<Args>(args)...);

      mTasksQueue.emplace([promise, func, boundArgs]() mutable {
        if constexpr (std::is_void_v<ReturnType>) {
          std::apply(std::forward<Fn>(func), std::move(args));
          promise->set_value();
        } else {
          promise->set_value(
              std::apply(std::forward<Fn>(func), std::move(boundArgs)));
        }
      });
    }

    mTasksQueueMutexCondition.notify_one();
    return result;
  }

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = default;
  ThreadPool &operator=(ThreadPool &&) = default;

private:
  std::vector<std::thread> mWorkers;

  std::queue<TaskFn> mTasksQueue;
  std::mutex mTasksQueueMutex;
  std::condition_variable mTasksQueueMutexCondition;
  std::atomic<bool> mTerminate;
};

} // namespace quoll
