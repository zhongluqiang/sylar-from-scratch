/**
 * @file util.cpp
 * @author zhongluqiang(zhong.rs232@gmail.com)
 * @brief util函数实现
 * @version 0.1
 * @date 2021-06-08
 */

#include "util.h"
#include <unistd.h>
#include <sys/syscall.h>

namespace sylar {

pid_t GetThreadId() {
    return syscall(SYS_gettid); 
}

uint64_t GetFiberId() {
    return 0; 
}
} // namespace sylar