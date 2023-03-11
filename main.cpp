#include <thread_pool.hpp>

std::mutex mx;
void sum(int a, int b) {
  std::unique_lock<std::mutex>lock(mx);
  std::cout << "Sum " << a + b << "\n";
  return;
}

int main() {
  ThreadPool thread_pool(4);
  thread_pool.enqueue([](int a, int b){sum(a,b);},10,576);
  thread_pool.enqueue([](int a, int b){sum(a,b);},0,5);
  thread_pool.enqueue([](int a, int b){sum(a,b);},17,1);

  return 0;
}