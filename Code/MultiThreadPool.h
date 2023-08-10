#ifndef MULTITHREADPOOL_H
#define MULTITHREADPOOL_H

#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <vector>
#include "SafeQueue.h"

class MultiThreadPool
{
private:
    class ThreadWorker
    {
    private:
        int m_id;
        int queue_id;
        MultiThreadPool* m_pool;

    public:
        ThreadWorker(MultiThreadPool* pool, const int id, const int num) : m_pool(pool), m_id(id), queue_id(num) {}
        void operator()()
        {
            std::function<void()> func;
            bool dequeued;
            while (true)
            {
                if (!(dequeued = m_pool->m_queues[queue_id].dequeue(func))) break;
                if (dequeued)
                    func();
            }
        }
    };

    std::vector<SafeQueue<std::function<void()>>> m_queues;
    std::vector<std::thread> m_threads;

public:
    MultiThreadPool(const int n_threads = 4)
        : m_queues(n_threads), m_threads(std::vector<std::thread>(n_threads)) {
        for (int i = 0; i < m_threads.size(); ++i)
        {
            m_threads.at(i) = std::thread(ThreadWorker(this, i, i));
        }
    }
    MultiThreadPool(const MultiThreadPool&) = delete;
    MultiThreadPool(MultiThreadPool&&) = delete;
    MultiThreadPool& operator=(const MultiThreadPool&) = delete;
    MultiThreadPool& operator=(MultiThreadPool&&) = delete;

    ~MultiThreadPool() {
        for (auto& queue : m_queues) {
            queue.stop();
        }
        for (int i = 0; i < m_threads.size(); ++i)
        {
            if (m_threads.at(i).joinable())
            {
                m_threads.at(i).join();
            }
        }
    }

    template <typename F, typename... Args>
    auto AddTask(int id, F&& f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        std::function<void()> m_func = [task_ptr]()
        {
            (*task_ptr)();
        };

        m_queues[id].enqueue(m_func);

        return task_ptr->get_future();
    }
};
#endif