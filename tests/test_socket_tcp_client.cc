/**
 * @file test_socket_tcp_client.cc
 * @brief 测试Socket类，tcp客户端
 * @version 0.1
 * @date 2021-09-18
 */
#include<sylar/sylar.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_tcp_client() {
    int ret;

    auto socket = sylar::Socket::CreateTCPSocket();
    SYLAR_ASSERT(socket);

    auto addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:12345");
    SYLAR_ASSERT(addr);

    ret = socket->connect(addr);
    SYLAR_ASSERT(ret);

    SYLAR_LOG_INFO(g_logger) << "connect success, peer address: " << socket->getRemoteAddress()->toString();

    std::string buffer;
    buffer.resize(1024);
    socket->recv(&buffer[0], buffer.size());
    SYLAR_LOG_INFO(g_logger) << "recv: " << buffer;
    socket->close();

    return;
}

int main(int argc, char *argv[]) {
    sylar::EnvMgr::GetInstance()->init(argc, argv);
    sylar::Config::LoadFromConfDir(sylar::EnvMgr::GetInstance()->getConfigPath());

    sylar::IOManager iom;
    iom.schedule(&test_tcp_client);

    return 0;
}