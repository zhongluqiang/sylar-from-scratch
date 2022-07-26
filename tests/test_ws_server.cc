#include "sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run() {
    sylar::http::WSServer::ptr server(new sylar::http::WSServer);
    sylar::Address::ptr addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:8020");
    if(!addr) {
        SYLAR_LOG_ERROR(g_logger) << "get address error";
        return;
    }
    auto fun = [](sylar::http::HttpRequest::ptr header
                  ,sylar::http::WSFrameMessage::ptr msg
                  ,sylar::http::WSSession::ptr session) {
        session->sendMessage(msg);
        return 0;
    };

    server->getWSServletDispatch()->addServlet("/sylar", fun);
    while(!server->bind(addr)) {
        SYLAR_LOG_ERROR(g_logger) << "bind " << *addr << " fail";
        sleep(1);
    }
    server->start();
}

int main(int argc, char** argv) {
    sylar::EnvMgr::GetInstance()->init(argc, argv);
    sylar::Config::LoadFromConfDir(sylar::EnvMgr::GetInstance()->getConfigPath());
    
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}
