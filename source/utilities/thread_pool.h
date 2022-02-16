#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <deque>
#include <functional>
#include <future>

namespace utilities
{

/**
 * Enumerations
 */
enum class ThreadPoolStatus { Pause, Ready, Terminate };

// forward declarations
ThreadPoolStatus &getGlobalThreadPoolStatusOverride(void);
void setGlobalThreadPoolStatusOverride(const ThreadPoolStatus &status);

/**
 * This class implements a thread pool manager using std::futures
 */
template<typename T>
class ThreadPool final
{
public:

    /**
     * Constructor
     */
    ThreadPool(size_t maximumThreads)
    : m_maximumThreads(maximumThreads),
      m_status(ThreadPoolStatus::Ready)
    {

    }

    /**
     * Copy constructor
     */
    ThreadPool(const ThreadPool<T> &pool)
    : m_maximumThreads(pool.m_maximumThreads),
      m_status(pool.m_status),
      m_tasks(pool.m_tasks)
    {

    }

    /**
     * Move constructor
     */
    ThreadPool(ThreadPool<T> &&pool)
    : m_maximumThreads(std::move(pool.m_maximumThreads)),
      m_status(std::move(pool.m_status)),
      m_tasks(std::move(pool.m_tasks))
    {

    }

    /**
     * Destructor
     */
    virtual ~ThreadPool(void)
    {

    }

    /**
     * Add a task
     */
    inline virtual void addTask(const std::function<T (void)> &task) final
    {
        m_tasks.push_back(task);
    }

    /**
     * Function to execute the thread pool
     */
    template<typename U = T> typename std::enable_if<!std::is_same<U, bool>::value, bool>::type execute(void)
    {
        return execute([] (const T &) { return true; });
    }

    /**
     * Function to execute the thread pool
     */
    template<typename U = T> typename std::enable_if<std::is_same<U, bool>::value, bool>::type execute(void)
    {
        return execute([] (const T &result) { return result; });
    }

    /**
     * Function to execute the thread pool
     * @param resultFunctor an std::function object that analyzes the result of each std::future object. The
     *                      functor should return true if thread pool execution should continue or false if it
     *                      should immediately terminate
     */
    virtual bool execute(const std::function<bool (const T &)> resultFunctor) final
    {
        bool bSuccess = true;

        // determine the number of threads to be created - depending upon the
        // number of tasks, the maximum number of threads may not be needed.
        auto &&numTasks = m_tasks.size();
        auto numThreads = std::min(m_maximumThreads, numTasks);
        auto &&thread = [&] (size_t threadId) -> T
        {
            for (size_t j = threadId; bSuccess && j < numTasks; j += numThreads)
            {
                while (getStatus() == ThreadPoolStatus::Pause); // block while paused

                auto &&task = m_tasks[j];
                if (getStatus() == ThreadPoolStatus::Terminate) // break if status is terminate
                    break;
                else // status is ready
                {
                    T &&result = task();
                    bSuccess &= resultFunctor(result);
                }
            }

            return bSuccess;
        };

        // start the threads
        std::vector<std::future<T>> threads(numThreads);
        for (size_t i = 0; i < numThreads; ++i)
            threads[i] = std::async(std::launch::async, thread, i);

        // retrieve the result of each thread, block if necessary
        for (size_t i = 0; i < numThreads; ++i)
            if (threads[i].valid())
                bSuccess &= threads[i].get();

        return bSuccess;
    }

    /**
     * Get the number of concurrent threads
     */
    inline virtual size_t getMaximumThreads(void) const final
    {
        return m_maximumThreads;
    }

    /**
     * Get this thread pool's status
     */
    inline virtual ThreadPoolStatus getStatus(void) const final
    {
        auto status = getGlobalThreadPoolStatusOverride();

        // if the global status is anything other than ready, return the global status
        return (status != ThreadPoolStatus::Ready) ? status : m_status;
    }

    /**
     * Get outstanding tasks
     */
    inline virtual std::deque<std::function<T (void)>> &getTasks(void) final
    {
        return m_tasks;
    }

    /**
     * Set the number of concurrent threads
     */
    inline virtual void setMaximumThreads(size_t maximumThreads) final
    {
        m_maximumThreads = maximumThreads;
    }

    /**
     * Set the polling interval (ms)
     */
    inline virtual void setStatus(const ThreadPoolStatus &status) final
    {
        m_status = status;
    }

private:

    /**
     * maximum number of threads to be created
     */
    size_t m_maximumThreads;

    /**
     * status of this thread pool
     */
    ThreadPoolStatus m_status;

    /**
     * vector of tasks, where the argument is the thread id
     */
    std::deque<std::function<T (void)>> m_tasks;
};

/**
 * Function to query the global thread pool status override
 */
inline ThreadPoolStatus &getGlobalThreadPoolStatusOverride(void)
{
    static ThreadPoolStatus status = ThreadPoolStatus::Ready;

    return status;
}

/**
 * Function to set the global thread pool status override
 */
inline void setGlobalThreadPoolStatusOverride(const ThreadPoolStatus &status)
{
    getGlobalThreadPoolStatusOverride() = status;
}

}

#endif
