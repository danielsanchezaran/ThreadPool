#include <thread_pool.hpp>

ThreadPool::ThreadPool(int numThreads) : stop(false), active_threads(0) {
  for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back([this] {
      for (;;) {
        std::function<void()> task;

        {
          std::unique_lock<std::mutex> lock(this->queueMutex);
          this->condition.wait(
              lock, [this] { return this->stop || !this->tasks.empty(); });
          if (this->stop && this->tasks.empty()) {
            --active_threads;
            return;
          }
          task = std::move(this->tasks.front());
          this->tasks.pop();
        }
        ++active_threads;
        task();
        --active_threads;
        condition.notify_all();
      }
    });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    stop = true;
  }
  condition.notify_all();
  for (std::thread& thread : threads) thread.join();
}

void ThreadPool::waitUntilDone() {
  std::unique_lock<std::mutex> lock(queueMutex);
  condition.wait(lock, [this] { return tasks.empty() && active_threads == 0; });
}