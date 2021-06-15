/**
 * @file fiber.h
 * @brief 协程模块
 * @details 基于ucontext_t实现，非对称协程
 * @version 0.1
 * @date 2021-06-15
 */

#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <functional>
#include <memory>
#include <ucontext.h>
#include "thread.h"

namespace sylar {

/**
 * @brief 协程类
 */
class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;

    /**
     * @brief 协程状态
     * @details 在sylar基础上进行了状态简化，只定义三态转换关系，也就是协程要么正在运行(RUNNING)，
     * 要么准备运行(READY)，要么运行结束(TERM)。不区分协程的初始状态，初始即READY。不区分协程是异常结束还是正常结束，
     * 只要结束就是TERM状态。也不区别HOLD状态，协程只要未结束也非运行态，那就是READY状态。
     */
    enum State {
        /// 就绪态，刚创建或者yield之后的状态
        READY,
        /// 运行态，resume之后的状态
        RUNNING,
        /// 结束态，协程的回调函数执行完之后为TERM状态
        TERM
    };

private:
    /**
     * @brief 构造函数
     * @attention 无参构造函数只用于创建线程的第一个协程，也就是线程主函数对应的协程，
     * 这个协程只能由GetThis()方法调用，所以定义成私有方法
     */
    Fiber();

public:
    /**
     * @brief 构造函数，用于创建用户协程
     * @param[in] cb 协程入口函数
     * @param[in] stacksize 栈大小
     * @param[in] run_in_scheduler 本协程是否参与调度器调度，默认为true
     */
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool run_in_scheduler = true);

    /**
     * @brief 析构函数
     */
    ~Fiber();

    /**
     * @brief 重置协程状态和入口函数，复用栈空间，不重新创建栈
     * @param[in] cb 
     */
    void reset(std::function<void()> cb);

    /**
     * @brief 将当前协程切到到执行状态
     * @details 当前协程和正在运行的协程进行交换，前者状态变为RUNNING，后者状态变为READY
     */
    void resume();

    /**
     * @brief 当前协程让出执行权
     * @details 当前协程与上次resume时退到后台的协程进行交换，前者状态变为READY，后者状态变为RUNNING
     */
    void yield();

    /**
     * @brief 获取协程ID
     */
    uint64_t getId() const { return m_id; }

    /**
     * @brief 获取协程状态
     */
    State getState() const { return m_state; }

public:
    /**
     * @brief 设置当前正在运行的协程，即设置线程局部变量t_fiber的值
     */
    static void SetThis(Fiber *f);

    /**
     * @brief 返回当前线程正在执行的协程
     * @details 如果当前线程还未创建协程，则创建线程的第一个协程，
     * 且该协程为当前线程的主协程，其他协程都通过这个协程来调度，也就是说，其他协程
     * 结束时,都要切回到主协程，由主协程重新选择新的协程进行resume
     * @attention 线程如果要创建协程，那么应该首先执行一下Fiber::GetThis()操作，以初始化主函数协程
     */
    static Fiber::ptr GetThis();

    /**
     * @brief 获取总协程数
     */
    static uint64_t TotalFibers();

    /**
     * @brief 协程入口函数
     */
    static void MainFunc();

    /**
     * @brief 获取当前协程id
     */
    static uint64_t GetFiberId();

private:
    /// 协程id
    uint64_t m_id        = 0;
    /// 协程栈大小
    uint32_t m_stacksize = 0;
    /// 协程状态
    State m_state        = READY;
    /// 协程上下文
    ucontext_t m_ctx;
    /// 协程栈地址
    void *m_stack = nullptr;
    /// 协程入口函数
    std::function<void()> m_cb;
    /// 本协程是否参与调度器调度
    bool m_runInScheduler;
};

} // namespace sylar

#endif