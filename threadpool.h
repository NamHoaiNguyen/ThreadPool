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
        SafeQueue<std::function<void()>> task_;

        std::vector<std::thread> worker_process_;

        std::condition_variable cv_;

        std::mutex mutex_;

        std::size_t num_threads_;

        bool shutdown_;

    public:
        ThreadPool() = delete;
    
        explicit ThreadPool(std::size_t num_threads) : num_threads_(num_threads), worker_process_(std::vector<std::thread>(num_threads)), shutdown_(false) {

        }

        void init();

        void shutdown();

        template<typename F, typename... Args>
        // decltype(auto) enqueue(T&& f, Args&&... args);
        auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type>;
    friend class Worker;
};

class Worker {
    private:
    //    std::unique_ptr<ThreadPool> pool_;
        ThreadPool *pool_;

    public:
        explicit Worker(ThreadPool *pool) : pool_(pool) {

        }

        void operator()() {
            std::function<void()> func;
            std::unique_lock<std::mutex> lock(pool_->mutex_);
            while (!pool_->shutdown_) {
                if (pool_->task_.empty()) {
                    pool_->cv_.wait(lock);
                }
                bool dequeued = pool_->task_.dequeue(func);
                if (dequeued) {
                    func();
                }
            }
        }
};

void ThreadPool::init() {
    for (auto i = 0; i < worker_process_.size(); i++) {
        worker_process_[i] = std::thread(Worker(this));
    }
}

template<typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result<F, Args...>::type> {
    // using return_type = typename std::invoke_result<T, Args...>::type;
    std::function<decltype(f(args...))()> wrapper_task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    // std::packaged_task<decltype(f(args...))()> package_task(wrapper_task);
    auto ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(wrapper_task);
    std::function<void()> object = [ptr](){(*ptr)();};

    task_.enqueue(object);

    cv_.notify_one();

    return ptr->get_future();
}

void ThreadPool::shutdown() {
    shutdown_ = true;
    cv_.notify_all();

    for (auto i = 0; i < worker_process_.size(); i++) {
        if (worker_process_[i].joinable()) {
            worker_process_[i].join();
        }
    }
}

#endif