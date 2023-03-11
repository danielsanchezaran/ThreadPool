#include <thread_safe_counter.hpp>

ThreadSafeCounter::ThreadSafeCounter() : count(0) {}

void ThreadSafeCounter::increment() {
  std::lock_guard<std::mutex> lock(mutex);
  ++count;
}

void ThreadSafeCounter::decrement() {
  std::lock_guard<std::mutex> lock(mutex);
  --count;
}

int ThreadSafeCounter::getCount() const {
  std::lock_guard<std::mutex> lock(mutex);
  return count;
}
