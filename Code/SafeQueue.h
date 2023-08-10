#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <vector>

template <typename T>
class SafeQueue {
public:
    SafeQueue() {}
    SafeQueue(SafeQueue&& t) {}
    ~SafeQueue() {}


    bool empty() {
        std::unique_lock<std::mutex> lock(m_mutex);

        return m_queue.empty();
    }

    int size() {
        std::unique_lock<std::mutex> lock(m_mutex);

        return m_queue.size();
    }

    void enqueue(const T& item) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.emplace(item);
        }
        cond_.notify_one();
    }

    void enqueue(T&& item) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.emplace(std::move(item));
        }
        cond_.notify_one();
    }
    bool dequeue(T& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        cond_.wait(lock, [&]() {
            return !m_queue.empty() || stop_;
            });
        if (m_queue.empty())
            return false;
        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            stop_ = true;
        }
        cond_.notify_all();
    }

private:
    std::queue<T> m_queue;
    std::condition_variable cond_;
    bool stop_ = false;
    std::mutex m_mutex;
};


#endif