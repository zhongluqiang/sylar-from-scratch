/**
 * @file test_hook.cc
 * @brief hook模块测试
 * @version 0.1
 * @date 2021-06-21
 */

#include "sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_sleep() {
    SYLAR_LOG_INFO(g_logger) << "test_sleep begin";
    sylar::IOManager iom;
    
    iom.schedule([] {
        sleep(2);
        SYLAR_LOG_INFO(g_logger) << "sleep 2";
    });

    iom.schedule([] {
        sleep(3);
        SYLAR_LOG_INFO(g_logger) << "sleep 3";
    });

    SYLAR_LOG_INFO(g_logger) << "test_sleep end";
}

int main(int argc, char *argv[]) {
    sylar::EnvMgr::GetInstance()->init(argc, argv);
    sylar::Config::LoadFromConfDir(sylar::EnvMgr::GetInstance()->getConfigPath());

    test_sleep();

    SYLAR_LOG_INFO(g_logger) << "main end";
    return 0;
}