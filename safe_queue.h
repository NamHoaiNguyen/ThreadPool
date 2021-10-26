#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <mutex>
#include <queue> 

template<typename T>
class Safe_Queue {
private:
    std::queue<T> queue_;

    std::mutex mutex_;

public:
    Safe_Queue() = default;

    ~Safe_Queue() = default;

    bool empty();

    int size();

    void enqueue(T& t);

    bool dequeue(T& t);
};

template<typename T>
bool Safe_Queue<T>::empty() {

}

template<typename T>
int Safe_Queue<T>::size() {

}

template<typename T>
void Safe_Queue<T>::enqueue(T& t) {

}

template<typename T>
bool Safe_Queue<T>::dequeue(T& t) {
    
}

#endif