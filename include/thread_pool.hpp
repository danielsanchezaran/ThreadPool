#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
/**
 * Creates a thread pool with numThreads concurrent threads, any type of
 * functions can be enqueued to the task queue. The class uses mutex and locks
 * to guarantee thread safety. Iit is important to note that in does not handle
 * thread safety for tasks with shared resources.
 */
class ThreadPool {
 public:
  explicit ThreadPool(int numThreads);
  ~ThreadPool();

  /*Enqueues a function along with its arguments to be called later in one of
   * the pool's threads
   * @param f: the function to call
   * @param args: arguments for f
   */
  template <class F, class... Args>
inline void enqueue(F&& f, Args&&... args) {
  {
    std::unique_lock<std::mutex> lock(queueMutex);
    tasks.emplace([=] { std::invoke(f, args...); });
  }
  condition.notify_one();
};

 private:
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> tasks;
  std::mutex queueMutex;
  std::condition_variable condition;
  bool stop;
};
