/**
 * @file scheduler.h
 * @brief 协程调度器实现
 * @version 0.1
 * @date 2021-06-15
 */

#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__

#include <functional>
#include <list>
#include <memory>
#include <string>
#include "fiber.h"
#include "log.h"
#include "thread.h"

namespace sylar {

/**
 * @brief 协程调度器
 * @details 封装的是N-M的协程调度器
 *          内部有一个线程池,支持协程在线程池里面切换
 */
class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    /**
     * @brief 创建调度器
     * @param[in] threads 线程数
     * @param[in] use_caller 是否将当前线程也作为调度线程
     * @param[in] name 名称
     */
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string &name = "Scheduler");

    /**
     * @brief 析构函数
     */
    virtual ~Scheduler();

    /**
     * @brief 获取调度器名称
     */
    const std::string &getName() const { return m_name; }

    /**
     * @brief 获取当前线程调度器指针
     */
    static Scheduler *GetThis();

    /**
     * @brief 获取当前线程的主协程
     */
    static Fiber *GetMainFiber();

    /**
     * @brief 添加调度任务
     * @tparam FiberOrCb 调度任务类型，可以是协程对象或函数指针
     * @param[] fc 协程对象或指针
     * @param[] thread 指定运行该任务的线程号，-1表示任意线程
     */
    template <class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }

        if (need_tickle) {
            tickle(); // 唤醒idle协程
        }
    }

    /**
     * @brief 启动调度器
     */
    void start();

    /**
     * @brief 停止调度器，等所有调度任务都执行完了再返回
     */
    void stop();

protected:
    /**
     * @brief 通知协程调度器有任务了
     */
    virtual void tickle();

    /**
     * @brief 协程调度函数
     */
    void run();

    /**
     * @brief 无任务调度时执行idle协程
     */
    virtual void idle();

    /**
     * @brief 返回是否可以停止
     */
    virtual bool stopping();

    /**
     * @brief 设置当前的协程调度器
     */
    void setThis();

    /**
     * @brief 返回是否有空闲线程
     * @details 当调度协程进入idle时空闲线程数加1，从idle协程返回时空闲线程数减1
     */
    bool hasIdleThreads() { return m_idleThreadCount > 0; }

private:
    /**
     * @brief 添加调度任务，无锁
     * @tparam FiberOrCb 调度任务类型，可以是协程对象或函数指针
     * @param[] fc 协程对象或指针
     * @param[] thread 指定运行该任务的线程号，-1表示任意线程
     */
    template <class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        bool need_tickle = m_tasks.empty();
        ScheduleTask task(fc, thread);
        if (task.fiber || task.cb) {
            m_tasks.push_back(task);
        }
        return need_tickle;
    }

private:
    /**
     * @brief 调度任务，协程/函数二选一，可指定在哪个线程上调度
     */
    struct ScheduleTask {
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;

        ScheduleTask(Fiber::ptr f, int thr) {
            fiber  = f;
            thread = thr;
        }
        ScheduleTask(Fiber::ptr *f, int thr) {
            fiber.swap(*f);
            thread = thr;
        }
        ScheduleTask(std::function<void()> f, int thr) {
            cb     = f;
            thread = thr;
        }
        ScheduleTask() { thread = -1; }

        void reset() {
            fiber  = nullptr;
            cb     = nullptr;
            thread = -1;
        }
    };

private:
    /// 协程调度器名称
    std::string m_name;
    /// 互斥锁
    MutexType m_mutex;
    /// 线程池
    std::vector<Thread::ptr> m_threads;
    /// 任务队列
    std::list<ScheduleTask> m_tasks;
    /// 线程池的线程ID数组
    std::vector<int> m_threadIds;
    /// 工作线程数量，不包含use_caller的主线程
    size_t m_threadCount = 0;
    /// 活跃线程数
    std::atomic<size_t> m_activeThreadCount = {0};
    /// idle线程数
    std::atomic<size_t> m_idleThreadCount = {0};

    /// 是否use caller
    bool m_useCaller;
    /// use_caller为true时，调度器所在线程的调度协程
    Fiber::ptr m_rootFiber;
    /// use_caller为true时，调度器所在线程的id
    int m_rootThread = 0;

    /// 是否正在停止
    bool m_stopping = false;
};

} // end namespace sylar

#endif
