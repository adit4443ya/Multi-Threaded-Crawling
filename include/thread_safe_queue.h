#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue
{
private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable cond;

public:
    void push(T value);
    bool try_pop(T& value);
    bool empty() const;
    size_t size() const;
};

template <typename T>
void ThreadSafeQueue<T>::push(T value)
{
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(std::move(value));
    cond.notify_one();
}

template <typename T>
bool ThreadSafeQueue<T>::try_pop(T& value)
{
    std::lock_guard<std::mutex> lock(mutex);
    if (queue.empty())
    {
        return false;
    }
    value = std::move(queue.front());
    queue.pop();
    return true;
}

template <typename T>
bool ThreadSafeQueue<T>::empty() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return queue.empty();
}

template <typename T>
size_t ThreadSafeQueue<T>::size() const
{
    std::lock_guard<std::mutex> lock(mutex);
    return queue.size();
}