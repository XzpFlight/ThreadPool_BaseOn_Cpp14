#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <vector>
#include "SafeQueue.h"

class ThreadPool
{
private:
    class ThreadWorker
    {
    private:
        int m_id;
        ThreadPool* m_pool;

    public:
        ThreadWorker(ThreadPool* pool, const int id) : m_pool(pool), m_id(id) {}
        void operator()()
        {
            std::function<void()> func;
            bool dequeued;
            while (true)
            {
                if (!(dequeued = m_pool->m_queue.dequeue(func))) break;
                if (dequeued)
                    func();
            }
        }
    };

    SafeQueue<std::function<void()>> m_queue;
    std::vector<std::thread> m_threads;

public:
    ThreadPool(const int n_threads = 4)
        : m_threads(std::vector<std::thread>(n_threads)) {
        for (int i = 0; i < m_threads.size(); ++i)
        {
            m_threads.at(i) = std::thread(ThreadWorker(this, i));
        }
    }
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    ~ThreadPool() {
        m_queue.stop();
        for (int i = 0; i < m_threads.size(); ++i)
        {
            if (m_threads.at(i).joinable())
            {
                m_threads.at(i).join();
            }
        }
    }

    template <typename F, typename... Args>
    auto AddTask(F&& f, Args &&...args) -> std::future<decltype(f(args...))>
    {
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        std::function<void()> m_func = [task_ptr]()
        {
            (*task_ptr)();
        };

        m_queue.enqueue(m_func);

        return task_ptr->get_future();
    }
};
#endif

