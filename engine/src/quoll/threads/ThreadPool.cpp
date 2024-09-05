#include "quoll/core/Base.h"
#include "ThreadPool.h"

namespace quoll {

ThreadPool::ThreadPool(u32 numThreads) {
  mWorkers.reserve(numThreads);

  for (u32 i = 0; i < numThreads; ++i) {
    mWorkers.emplace_back([this] {
      while (true) {
        TaskFn task;
        {
          std::unique_lock<std::mutex> lock(mTasksQueueMutex);
          mTasksQueueMutexCondition.wait(lock, [this] {
            return mTerminate.load() || !mTasksQueue.empty();
          });

          if (mTerminate) {
            return;
          }

          task = std::move(mTasksQueue.front());
          mTasksQueue.pop();
        }

        task();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  mTerminate.store(true);
  mTasksQueueMutexCondition.notify_all();
  for (auto &worker : mWorkers) {
    worker.join();
  }
}

} // namespace quoll
