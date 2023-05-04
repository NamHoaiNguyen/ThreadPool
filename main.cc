#include <iostream>
#include <random>

#include "thread_pool.h"

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(-1000, 1000);
auto rnd = std::bind(dist, mt);


void simulate_hard_computation() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2000 + rnd()));
}

// Simple function that adds multiplies two numbers and prints the result
void multiply(const int a, const int b) {
  simulate_hard_computation();
  const int res = a * b;
  std::cout << a << " * " << b << " = " << res << std::endl;
}

// Same as before but now we have an output parameter
void multiply_output(int & out, const int a, const int b) {
  simulate_hard_computation();
  out = a * b;
  std::cout << a << " * " << b << " = " << out << std::endl;
}

// Same as before but now we have an output parameter
int multiply_return(const int a, const int b) {
  simulate_hard_computation();
  const int res = a * b;
  std::cout << a << " * " << b << " = " << res << std::endl;
  return res;
}


int main(int argc, char *argv[])
{
  using namespace std::placeholders;

  // Create pool with 3 threads
  threadpool::ThreadPool pool;

  // Initialize pool
  pool.init();

  // Submit (partial) multiplication table
  for (int i = 1; i < 3; ++i) {
    for (int j = 1; j < 10; ++j) {
      pool.submit(multiply, i, j);
    }
  }

  // Submit function with output parameter passed by ref
  int output_ref;
  auto future1 = pool.submit(multiply_output, std::ref(output_ref), 5, 6);

  // Wait for multiplication output to finish
  future1.get();
  std::cout << "Last operation result is equals to " << output_ref << std::endl;

  // Submit function with return parameter 
  auto future2 = pool.submit(multiply_return, 5, 3);

  // Wait for multiplication output to finish
  int res = future2.get();
  std::cout << "Last operation result is equals to " << res << std::endl;
  
  struct Test {
    int a_;
    int b_;

    struct Nest {
      int x_;
      int y_;

      int add(int a, int b) {
        return a + b;
      }
    };

    int add(int a, int b) {
      return a + b;
    }
    int uniquePtrAdd(std::unique_ptr<int> a, std::unique_ptr<int>  b) {
      return *a + *b;
    }
  };

  //Test with class method
  Test test;
  Test::Nest nest;
  auto future3 = pool.submit(&Test::Nest::add, nest, 5, 3);
  int res3 = future3.get();
  std::cout << "Last operation result is equals to " << res3 << std::endl;

  // auto a = std::make_unique<int>(10);
  // auto b = std::make_unique<int>(20);

  // auto future4 = pool.submit(&Test::uniquePtrAdd, test, std::move(a), std::move(b));
  // int res4 = future4.get();
  // std::cout << "Last operation result is equals to " << res4 << std::endl;

  // std::function<int()> functor = std::bind(&Test::uniquePtrAdd, test, std::move(a), std::move(b));
  // test.uniquePtrAdd(std::move(a), std::move(b));

  pool.shutdown();

  return 0;
}