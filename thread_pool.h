#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "safe_queue.h"
#include "wrapper.h"

namespace threadpool {

class ThreadPool {
  private:
    //Number of threads in thread pool.
    int thread_pool_size_;

    //Whether to shutdown server or not.
    bool shutdown_;

    //Condition variable to notify thread poll that new taks to execute.
    std::condition_variable cv_;

    //Mutex to lock task queues.
    std::mutex queue_mutex_;

    //Queue containing task needed to exectuted.
    SafeQueue<std::function<void()>> task_queue_;

    //List of thread in thread pool.
    std::vector<std::thread> worker_thread_;

    // ThreadWorker takes taks from task queue and notify one(or all) thread in
    // thread pool.
    class ThreadWorker {
      private:
        int m_id;
        ThreadPool *m_pool;

      public:
        ThreadWorker(ThreadPool *pool, const int id) : m_pool(pool), m_id(id) {}

        void operator()() {
          std::function<void()> func;
          bool dequeued;
          while (!m_pool->shutdown_) {
            {
              std::unique_lock<std::mutex> lock(m_pool->queue_mutex_);

              if (m_pool->task_queue_.empty()) {
                m_pool->cv_.wait(lock);
              }
              dequeued = m_pool->task_queue_.dequeue(func);
            }
            if (dequeued)
              func();
          }
        }
    };

  public:
    ThreadPool(int num_threads)
        : thread_pool_size_(num_threads)
          worker_thread_(std::vector<std::thread>(thread_pool_size_)),
          shutdown_(false) {

    }

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;

    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

    void init() {
      for(int i = 0; i < thread_pool_size_; i++) {
        worker_thread_[i] = std::thread(ThreadWorker(this, i));
      }
    }

    void shutdown() {
      if (shutdown_)
        return;

      //Must set before join thread.
      //Otherwise, the thread can be joined when executing task.
      shutdown_ = true;

      //Unblock all threads.
      cv_.notify_all();
      
      for (int i = 0; i < thread_pool_size_; i++) {
        if (worker_thread_[i].joinable()) {
          worker_thread_[i].join();
        }
      }
    }

    // Submit function into task queue to be executed asynchronously by the pool
    // Return type is std::future
    template<typename F, typename... Args>
    decltype(auto) submit(F&& f, Args&&... args) {
      //Type is a std::function.
      auto func =
          wrapper::wrapper(std::forward<F>(f), std::forward<Args>(args)...);
      // Encapsulate it into a shared ptr in order to be able to copy construct
      auto task_ptr =
          std::make_shared<std::packaged_task<wrapper::ReturnType<F>()>>(func);

      // Wrap packaged task into void function
      std::function<void()> wrapper_func = [task_ptr](){
        (*task_ptr)();
      };

      //Put task into queue.
      task_queue_.enqueue(wrapper_func);

      //Wake up one thread to notify that new task need to be executed.
      cv_.notify_one();

      //Return future.
      return task_ptr->get_future();
    }
};

} //namespace ThreadPool

#endif // 