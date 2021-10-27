#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

#include "safe_queue.h"

class ThreadPool {
private:
    std::vector<std::thread> workers_; 

    // template<typename T, typename... Args>
    // using tasks_ = SafeQueue<std::function<T(Args... args)>>;

    SafeQueue<std::function<void()>> tasks_;

    std::mutex mutex_;

    std::condition_variable condition_;

    bool shutdown_;

public:
    explicit ThreadPool(std::size_t);

    ~ThreadPool() = default;

    ThreadPool(const ThreadPool&) = delete;

    ThreadPool& operator=(const ThreadPool&) = delete;

    ThreadPool(ThreadPool&&) = delete;

    ThreadPool& operator=(ThreadPool &&) = delete;

    template<typename T, typename... Args>
    decltype(auto) enqueue(T&& t, Args&&... args);
};

ThreadPool::ThreadPool(std::size_t n_threads) : workers_(std::vector<std::thread>(n_threads)), shutdown_(false) {
    for (std::size_t i = 0; i < n_threads; i++) {
        // workers_.emplace_back(
        //     []() {

        //     }
        // );
    }
}

template<typename T, typename... Args>
decltype(auto) ThreadPool::enqueue(T&& t, Args&&... args) {
    using return_type = typename std::invoke_result<T(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<T(Args...)>>(
            std::bind(std::forward<T>(t), std::forward<Args>(args)...)); 

    std::future<void()> object = task->get.future();

    std::function<T(Args...)> wrapper_func = [task]() {
        return (*task)();
    };

    tasks_.enqueue(wrapper_func);

    condition_.notify_one();

    return object;
}

#endif