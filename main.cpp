#include <thread_pool.hpp>
#include <thread_safe_counter.hpp>

std::mutex mx;
void sum(int a, int b) {
  std::unique_lock<std::mutex> lock(mx);
  std::cout << "Sum " << a + b << "\n";
  return;
}

int main() {
  constexpr int numThreads = 4;
  constexpr int numTasks = 1000;
  ThreadPool thread_pool(numThreads);
  ThreadSafeCounter counter;
  // Its possible to enqueue lambda funcs, defined functions, and also get a
  // std::future() to get the result of an enqueued funtion when its ready.
  thread_pool.enqueue([](int a, int b) { sum(a, b); }, 10, 576);
  thread_pool.enqueue(sum, 3, 6);
  auto future_result = thread_pool.enqueue_result([]() { return 10; });

  // Increase the counter 1000 times
  for (int i = 0; i < numTasks; ++i) {
    thread_pool.enqueue([&counter] { counter.increment(); });
  }

  thread_pool.waitUntilDone();
  std::cout << "Count to 1000: " << counter.getCount() << std::endl;

  for (int i = 0; i < numTasks; ++i) {
    thread_pool.enqueue([&counter] { counter.decrement(); });
  }

  thread_pool.waitUntilDone();
  std::cout << "Count to 0: " << counter.getCount() << std::endl;

  // Retrieve the result
  std::cout << "Future result " << future_result.get() << std::endl;
  return 0;
}