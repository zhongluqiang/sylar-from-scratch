/**
 * @file test_http_connection.cc
 * @brief HTTP客户端类测试
 * @version 0.1
 * @date 2021-12-09
 */
#include "sylar/sylar.h"
#include <iostream>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_pool() {
    sylar::http::HttpConnectionPool::ptr pool(new sylar::http::HttpConnectionPool(
        "www.midlane.top", "", 80, 10, 1000 * 30, 5));

    sylar::IOManager::GetThis()->addTimer(
        1000, [pool]() {
            auto r = pool->doGet("/", 300);
            std::cout << r->toString() << std::endl;
        },
        true);
}

void run() {
    sylar::Address::ptr addr = sylar::Address::LookupAnyIPAddress("www.midlane.top:80");
    if (!addr) {
        SYLAR_LOG_INFO(g_logger) << "get addr error";
        return;
    }

    sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
    bool rt                 = sock->connect(addr);
    if (!rt) {
        SYLAR_LOG_INFO(g_logger) << "connect " << *addr << " failed";
        return;
    }

    sylar::http::HttpConnection::ptr conn(new sylar::http::HttpConnection(sock));
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
    req->setPath("/");
    req->setHeader("host", "www.midlane.top");
    // 小bug，如果设置了keep-alive，那么要在使用前先调用一次init
    req->setHeader("connection", "keep-alive");
    req->init();
    std::cout << "req:" << std::endl
              << *req << std::endl;

    conn->sendRequest(req);
    auto rsp = conn->recvResponse();

    if (!rsp) {
        SYLAR_LOG_INFO(g_logger) << "recv response error";
        return;
    }
    std::cout << "rsp:" << std::endl
              << *rsp << std::endl;

    std::cout << "=========================" << std::endl;

    auto r = sylar::http::HttpConnection::DoGet("http://www.midlane.top/wiki/", 300);
    std::cout << "result=" << r->result
              << " error=" << r->error
              << " rsp=" << (r->response ? r->response->toString() : "")
              << std::endl;

    std::cout << "=========================" << std::endl;
    test_pool();
}

int main(int argc, char **argv) {
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
