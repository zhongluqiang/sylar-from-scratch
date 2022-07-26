/**
 * @file test_ws_web_server.cc
 * @brief 网页WebSocket Echo服务器
 *        这个示例用于展示如何结合http和websocket来一起构建网页应用
 *        有一点要注意，实际上http和WebSocket可以共用同一个端口，但sylar未实现这种方式
 * @version 0.1
 * @date 2022-07-26
 */
#include "sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

#define XX(...) #__VA_ARGS__

int32_t handleChat(sylar::http::HttpRequest::ptr req, sylar::http::HttpResponse::ptr rsp, sylar::http::HttpSession::ptr session) {
    rsp->setHeader("Content-Type", "text/html; charset=UTF-8");
    rsp->setBody(XX(
<html>
    <head>
        <title>WebSocket Echo</title>
    </head>
    <script type="text/javascript">
        var websocket = null;
        // 页面显示消息
        function write_msg(msg) {
            document.getElementById("message").innerHTML += msg + "<br/>";
        }
        // 网页加载完成后自动连接websocket
        window.onload = function() {
            websocket = new WebSocket("ws://localhost:8021/chat");

            websocket.onerror = function() {
                write_msg("onerror");
            };

            websocket.onopen = function() {
                write_msg("WebSocket连接成功");
            };

            websocket.onmessage= function(msg) {
                //console.log(msg);
                write_msg(msg.data);
            };

            websocket.onclose= function() {
                write_msg("服务器断开");
            };
        };
        // 点击发送消息
        function sendmsg() {
            var msg = document.getElementById('msg').value;
            websocket.send(msg);
        }
    </script>
    <body>
        <input id="msg" type="text"/><button onclick="sendmsg()">发送</button><br/>
        <div id="message">
        </div>
    </body>
</html>
        ));
        
    return 0;
}

void run_http_server() {
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(true));
    sylar::Address::ptr addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:8020");
    while (!server->bind(addr)) {
        sleep(2);
    }
    auto sd = server->getServletDispatch();
    sd->addServlet("/sylar/chat", handleChat);
    server->start();
}

void run_ws_server() {
    sylar::http::WSServer::ptr server(new sylar::http::WSServer);
    sylar::Address::ptr addr = sylar::Address::LookupAnyIPAddress("0.0.0.0:8021");
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

    server->getWSServletDispatch()->addServlet("/chat", fun);
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
    iom.schedule(run_http_server);
    iom.schedule(run_ws_server);
    return 0;
}
