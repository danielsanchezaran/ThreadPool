cmake_minimum_required (VERSION 3.14)
project (test)
set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories( ./include ./src)
 
add_executable (main main.cpp ./src/thread_pool.cpp ./src/thread_safe_counter.cpp)
target_link_libraries(main PRIVATE Threads::Threads)

enable_testing()

# Find and link Google Test library
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(thread_pool_test ./tests/thread_pool_test.cpp ./src/thread_pool.cpp)
target_include_directories(thread_pool_test PRIVATE ./include)
target_link_libraries(thread_pool_test PRIVATE Threads::Threads GTest::GTest GTest::Main)

add_test(NAME Thread_Pool_Test COMMAND thread_pool_test)