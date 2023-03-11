cmake_minimum_required (VERSION 3.14)
project (test)
set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories( ./include ./src)
 
add_executable (main main.cpp ./src/thread_pool.cpp ./src/thread_safe_counter.cpp)
target_link_libraries(main PRIVATE Threads::Threads)