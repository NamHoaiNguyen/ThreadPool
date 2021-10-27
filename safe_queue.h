#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <mutex>
#include <queue> 

template<typename T>
class SafeQueue {
private:
    std::queue<T> queue_;

    std::mutex mutex_;

public:
    SafeQueue() = default;

    ~SafeQueue() = default;

    bool empty();

    bool check_empty();

    int size();

    void enqueue(T& t);

    bool dequeue(T& t);
};

template<typename T>
bool SafeQueue<T>::empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return check_empty();
}

template<typename T>
bool SafeQueue<T>::check_empty() {
    return queue_.empty();
}

template<typename T>
int SafeQueue<T>::size() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}

template<typename T>
void SafeQueue<T>::enqueue(T& t) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(t);
}

template<typename T>
bool SafeQueue<T>::dequeue(T& t) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (queue_.empty()) {
        return false;
    }

    t = std::move(queue_.front());

    queue_.pop();
    return true;
}

#endif