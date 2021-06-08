/**
 * @file util.h
 * @author zhongluqiang(zhong.rs232@gmail.com)
 * @brief util函数
 * @version 0.1
 * @date 2021-06-08
 */

#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <sys/types.h>
#include <stdint.h>

namespace sylar {

/**
 * @brief 获取线程id
 * @note 这里不要把pid_t和pthread_t混淆，参考man 2 gettid
 */
pid_t GetThreadId();

/**
 * @brief 获取协程id
 * @attention 桩函数，暂时返回0，等协程模块完善后再返回实际值
 */
uint64_t GetFiberId();

} // namespace sylar

#endif // __SYLAR_UTIL_H__
