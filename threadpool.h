#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>s
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

class ThreadPool {
private:
    std::vector<std::thread> pool_; 

    template<typename T, typename... Args>
    using tasks_ = std::queue<std::function<T(Args... args)>>;

    std::mutex mutex_;

    std::condition_variable condition_;

    bool shutdown_;

public:
    explicit ThreadPool(std::size_t);

    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;

    ThreadPool& operator=(const ThreadPool&) = delete;

    ThreadPool(ThreadPool&&) = delete;

    ThreadPool& operator=(ThreadPool &&) = delete;

    template<typename T, typename... Args>
    decltype(auto) enqueue(T&& t, Args&&... args);
};

ThreadPool::ThreadPool(std::size_t n_threads) : pool_(std::vector<std::thread>(n_threads)), shutdown_(false) {

}

template<typename T, typename... Args>

#endif