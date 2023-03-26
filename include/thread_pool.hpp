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
      // don't allow enqueueing after stopping the pool
      if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");
      tasks.emplace([=] { std::invoke(f, args...); });
    }
    condition.notify_one();
  };

  /*Enqueues a function along with its arguments to be called later in one of
   * the pool's threads. Returns a std::future that can be used to obtain the
   * result of the function
   * @param f: the function to call
   * @param args: arguments for f
   * @return result of function
   */
  template <class F, class... Args>
  inline auto enqueue_result(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
      std::unique_lock<std::mutex> lock(queueMutex);

      // don't allow enqueueing after stopping the pool
      if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");

      tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
  }

  void waitUntilDone();

 private:
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> tasks;
  std::mutex queueMutex;
  std::condition_variable condition;
  bool stop;
  std::atomic<int> active_threads;
};
