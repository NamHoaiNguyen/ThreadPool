#include "threadpool.h"

#include <iostream>

int add(int a, int b) {
    const int res = a + b;
  std::cout << a << " + " << b << " = " << res << std::endl;
}

int add_output(int& out, int a, int b) {
    out =  a + b;
    std::cout << a << " + " << b << " = " << out << std::endl;
}

int main()
{
    ThreadPool pool(3);

    pool.init();

for (int i = 1; i < 3; ++i) {
    for (int j = 1; j < 10; ++j) {
      pool.enqueue(add, i, j);
    }
  }


    int output_ref;
    auto future1 = pool.enqueue(add_output, std::ref(output_ref), 1, 2);

    future1.get();
    std::cout << "Result: " << output_ref << std::endl;

    pool.shutdown();

    return 0;
}

// #include <functional>
// #include <iostream>
 
// struct Foo {
//     Foo(int num) : num_(num) {}
//     void print_add(int i) const { std::cout << num_+i << '\n'; }
//     int num_;
// };
 
// void print_num(int i)
// {
//     std::cout << i << '\n';
// }
 
// struct PrintNum {
//     void operator()(int i) const
//     {
//         std::cout << i << '\n';
//     }
// };
 
// int main()
// {
//     // store a free function
//     std::function<void(int)> f_display = print_num;
//     f_display(-9);
 
//     // store a lambda
//     std::function<void()> f_display_42 = []() { print_num(42); };
//     f_display_42();
// }