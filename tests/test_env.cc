/**
 * @file test_env.cc
 * @brief 环境变量测试
 * @version 0.1
 * @date 2021-06-13
 */
#include "sylar/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

sylar::Env *g_env = sylar::EnvMgr::GetInstance();

int main(int argc, char *argv[]) {
    g_env->addHelp("h", "print this help message");

    bool is_print_help = false;
    if(!g_env->init(argc, argv)) {
        is_print_help = true;
    }
    if(g_env->has("h")) {
        is_print_help = true;
    }

    if(is_print_help) {
        g_env->printHelp();
        return false;
    }

    SYLAR_LOG_INFO(g_logger)<< "exe: " << g_env->getExe();
    SYLAR_LOG_INFO(g_logger) <<"cwd: " << g_env->getCwd();
    SYLAR_LOG_INFO(g_logger) << "absoluth path of test: " << g_env->getAbsolutePath("test");
    SYLAR_LOG_INFO(g_logger) << "conf path:" << g_env->getConfigPath();

    g_env->add("key1", "value1");
    SYLAR_LOG_INFO(g_logger) << "key1: " << g_env->get("key1");

    g_env->setEnv("key2", "value2");
    SYLAR_LOG_INFO(g_logger) << "key2: " << g_env->getEnv("key2");

    SYLAR_LOG_INFO(g_logger) << g_env->getEnv("PATH");

    return 0;
}