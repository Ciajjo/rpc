#ifndef LOCKQUEUE_H
#define LOCKQUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <class T>
class LockQueue
{
public:
    void push(const T &data);
    T pop();

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

template <class T>
inline void LockQueue<T>::push(const T &data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(data);
    cond_.notify_one();
}

template <class T>
inline T LockQueue<T>::pop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]()
               { return !queue_.empty(); });
    T data = queue_.front();
    queue_.pop();
    return data;
    // TODO: 在此处插入 return 语句
}

#endif // LOCKQUEUE_H