#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <mutex>
#include <queue>

namespace threadpool {

template<typename T>
class SafeQueue {
  private:
    std::mutex queue_mutex_;
    std::queue<T> queue_;

  public:
    SafeQueue() = default;

    ~SafeQueue() = default;

    SafeQueue(const SafeQueue&) = delete;
    SafeQueue& operator=(const SafeQueue&) = delete;

    SafeQueue(SafeQueue&&) = delete;
    SafeQueue& operator=(SafeQueue&&) = delete;

    bool empty() {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      return queue_.empty();
    }

    int size() {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      return queue_.size();
    }

    void enqueue(T& t) {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      queue_.push(t);
    }

    bool dequeue(T& t) {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      if (queue_.empty()) 
        return false;
      
      t = std::move(queue_.front());
      queue_.pop();

      return true;
    }
};

} // namespace ThreadPool

#endif // SAFE_QUEUE_H