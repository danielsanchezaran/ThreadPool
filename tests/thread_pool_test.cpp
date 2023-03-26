#include <thread_pool.hpp>
#include <thread_safe_counter.hpp>

#include "gtest/gtest.h"

// Tests the basic functionality of ThreadPool
TEST(ThreadPoolTest, BasicTest) {
  ThreadPool pool(4);

  // Enqueue a task without result
  int result = 0;
  pool.enqueue([&result] { result = 1; });
  std::this_thread::sleep_for(
      std::chrono::milliseconds(100));  // wait for task to complete
  EXPECT_EQ(result, 1);

  // Enqueue a task with result
  auto future_result = pool.enqueue_result([] { return 2; });
  EXPECT_EQ(future_result.get(), 2);
}

// Tests that ThreadPool can handle multiple tasks and threads
TEST(ThreadPoolTest, MultipleTasksAndThreadsTest) {
  ThreadPool pool(4);

  // Enqueue multiple tasks
  std::vector<int> results(10);
  for (int i = 0; i < 10; i++) {
    pool.enqueue([&results, i] { results[i] = i * i; });
  }

  // Wait for all tasks to complete
  std::this_thread::sleep_for(
      std::chrono::milliseconds(100));  // wait for tasks to complete

  // Check results
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(results[i], i * i);
  }
}

// Tests that ThreadPool can handle tasks that take a long time to complete
TEST(ThreadPoolTest, LongTaskTest) {
  ThreadPool pool(4);

  // Enqueue a task that takes a long time to complete
  int result = 0;
  pool.enqueue([&result] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    result = 1;
  });
  EXPECT_EQ(result, 0);  // task hasn't completed yet
  std::this_thread::sleep_for(
      std::chrono::milliseconds(200));  // wait for task to complete
  EXPECT_EQ(result, 1);
}

// Tests that ThreadPool can handle tasks that return a value
TEST(ThreadPoolTest, ResultTest) {
  ThreadPool pool(4);

  // Enqueue a task that returns a value
  auto future_result = pool.enqueue_result([] { return 1; });
  EXPECT_EQ(future_result.get(), 1);
}

// Tests that ThreadPool can handle exceptions thrown by tasks
TEST(ThreadPoolTest, ExceptionTest) {
  ThreadPool pool(4);

  // Enqueue a task that throws an exception
  auto future_result =
      pool.enqueue_result([] { throw std::runtime_error("error"); });
  EXPECT_THROW(future_result.get(), std::runtime_error);
}


TEST(ThreadSafeCounterTest, IncrementAndDecrement) {
  ThreadSafeCounter counter;
  
  // Check that the initial count is 0
  EXPECT_EQ(counter.getCount(), 0);
  
  // Increment the counter twice and check that the count is 2
  counter.increment();
  EXPECT_EQ(counter.getCount(), 1);
  counter.increment();
  EXPECT_EQ(counter.getCount(), 2);
  
  // Decrement the counter twice and check that the count is 0
  counter.decrement();
  EXPECT_EQ(counter.getCount(), 1);
  counter.decrement();
  EXPECT_EQ(counter.getCount(), 0);
}

TEST(ThreadSafeCounterTest, ConcurrentAccess) {
  ThreadSafeCounter counter;
  constexpr int kNumThreads = 10;
  constexpr int kNumIterations = 1000000;
  std::vector<std::thread> threads;
  
  // Create kNumThreads threads, each incrementing and decrementing the counter kNumIterations times
  for (int i = 0; i < kNumThreads; ++i) {
    threads.emplace_back([&counter]() {
      for (int j = 0; j < kNumIterations; ++j) {
        counter.increment();
        counter.decrement();
      }
    });
  }
  
  // Wait for all threads to finish
  for (auto& thread : threads) {
    thread.join();
  }
  
  // Check that the final count is 0
  EXPECT_EQ(counter.getCount(), 0);
}

TEST(ThreadPoolTest, ThreadPoolWithCounterTest) {
  const int num_tasks = 10;
  ThreadPool pool{5};
  ThreadSafeCounter counter;

  // Enqueue tasks to increment the counter
  for (int i = 0; i < num_tasks; ++i) {
    pool.enqueue([&counter] { counter.increment(); });
  }

  // Wait for tasks to complete
  pool.waitUntilDone();

  // Check that the counter was incremented the expected number of times
  EXPECT_EQ(counter.getCount(), num_tasks);
}