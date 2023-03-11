#include <mutex>

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