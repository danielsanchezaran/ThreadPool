#include <mutex>

/**
 * A thread-safe counter that provides functions to increment, decrement, and
 * get the current count. The class uses mutex and locks to guarantee thread safety.
 */
class ThreadSafeCounter {
 public:
  ThreadSafeCounter();
  ~ThreadSafeCounter();
  void increment();
  void decrement();
  int getCount() const;

 private:
  mutable std::mutex mutex;
  int count;
};