#include <thread_pool.hpp>
#include "gtest/gtest.h"

// Tests the basic functionality of ThreadPool
TEST(ThreadPoolTest, BasicTest) {
  ThreadPool pool(4);

  // Enqueue a task without result
  int result = 0;
  pool.enqueue([&result] { result = 1; });
  std::this_thread::sleep_for(std::chrono::milliseconds(100));  // wait for task to complete
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
  std::this_thread::sleep_for(std::chrono::milliseconds(100));  // wait for tasks to complete

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
  std::this_thread::sleep_for(std::chrono::milliseconds(200));  // wait for task to complete
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
  auto future_result = pool.enqueue_result([] { throw std::runtime_error("error"); });
  EXPECT_THROW(future_result.get(), std::runtime_error);
}
