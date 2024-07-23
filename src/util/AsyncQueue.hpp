
#pragma once
#include <queue>
#include <mutex>
#include <optional>
#include <condition_variable>

template <class T>
class AsyncQueue
{
public:
    /**
     * Tries to grab a message from the queue, returns std::nullopt if the queue is empty
     */
    [[nodiscard]] std::optional<T> popFront()
    {
        std::lock_guard lk(mtx);
        if (!queue.empty())
        {
            T front = queue.front();
            queue.pop();
            return front;
        }
        else
            return std::nullopt;
    }
    /**
     * Waits for an item to be available in the queue and then retrieves it
     * @return Always a valid T
     */
    [[nodiscard]] T blockingPopFront()
    {
        std::unique_lock lk(mtx);
        cv.wait(lk, [&](){ return !queue.empty(); });
        T front = std::move(queue.front());
        queue.pop();
        return front;
    }
    void pop()
    {
        std::lock_guard lk(mtx);
        queue.pop();
    }
    void push(const T& item)
    {
        std::lock_guard lk(mtx);
        queue.push(item);
        cv.notify_one();
    }
    void emplace(T&& item)
    {
        std::lock_guard lk(mtx);
        queue.emplace(std::move(item));
        cv.notify_one();
    }
    [[nodiscard]] bool empty() const
    {
        std::lock_guard lk(mtx);
        return queue.empty();
    }
    [[nodiscard]] size_t size() const
    {
        std::lock_guard lk(mtx);
        return queue.size();
    }

private:
    std::queue<T> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
};
