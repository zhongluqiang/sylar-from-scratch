/**
 * @file util.h
 * @brief util函数
 * @version 0.1
 * @date 2021-06-08
 */

#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <sys/types.h>
#include <stdint.h>
#include <string>

namespace sylar {

/**
 * @brief 获取线程id
 * @note 这里不要把pid_t和pthread_t混淆，关于它们之的区别可参考gettid(2)
 */
pid_t GetThreadId();

/**
 * @brief 获取协程id
 * @todo 桩函数，暂时返回0，等协程模块完善后再返回实际值
 */
uint64_t GetFiberId();

/**
 * @brief 获取当前启动的毫秒数，参考clock_gettime(2)，使用CLOCK_MONOTONIC_RAW
 */
uint64_t GetElapsed();

/**
 * @brief 获取线程名称，参考pthread_getname_np(3)
 */
std::string GetThreadName();

/**
 * @brief 设置线程名称，参考pthread_setname_np(3)
 * @note 线程名称不能超过16字节，包括结尾的'\0'字符
 */
void SetThreadName(const std::string &name);

} // namespace sylar

#endif // __SYLAR_UTIL_H__
