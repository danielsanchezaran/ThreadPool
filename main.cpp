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
  thread_pool.enqueue([](int a, int b) { sum(a, b); }, 10, 576);
  thread_pool.enqueue([](int a, int b) { sum(a, b); }, 0, 5);
  thread_pool.enqueue([](int a, int b) { sum(a, b); }, 17, 1);

  for (int i = 0; i < numTasks; ++i) {
    thread_pool.enqueue([&counter] { counter.increment(); });
  }
  std::cout << "Count: " << counter.getCount() << std::endl;

  return 0;
}