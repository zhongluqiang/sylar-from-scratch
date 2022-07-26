/**
 * @file test_http_parser.cc
 * @brief 测试HTTP协议解析
 * @version 0.1
 * @date 2021-09-25
 */
#include "sylar/sylar.h"

const char test_request_data[] = "POST /login?aa=bb#sss HTTP/1.1\r\n"
                                 "Host: www.sylar.top\r\n"
                                 "Content-Length: 10\r\n\r\n"
                                 "1234567890";

const char test_request_chunked_data[] = "POST /two_chunks_mult_zero_end HTTP/1.1\r\n"
                                         "Transfer-Encoding: chunked\r\n"
                                         "\r\n"
                                         "5\r\nhello\r\n"
                                         "6\r\n world\r\n"
                                         "000\r\n"
                                         "\r\n";

const char test_response_data[] = "HTTP/1.1 301 Moved Permanently\r\n"
                                  "Location: http://www.google.com/\r\n"
                                  "Content-Type: text/html; charset=UTF-8\r\n"
                                  "Date: Sun, 26 Apr 2009 11:11:49 GMT\r\n"
                                  "Expires: Tue, 26 May 2009 11:11:49 GMT\r\n"
                                  "X-$PrototypeBI-Version: 1.6.0.3\r\n" /* $ char in header field */
                                  "Cache-Control: public, max-age=2592000\r\n"
                                  "Server: gws\r\n"
                                  "Content-Length:  219  \r\n"
                                  "\r\n"
                                  "<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n"
                                  "<TITLE>301 Moved</TITLE></HEAD><BODY>\n"
                                  "<H1>301 Moved</H1>\n"
                                  "The document has moved\n"
                                  "<A HREF=\"http://www.google.com/\">here</A>.\r\n"
                                  "</BODY></HTML>\r\n";

void test_request(const char *str) {
    sylar::http::HttpRequestParser parser;
    std::string tmp = str;
    std::cout << "<test_request>:" << std::endl
              << tmp << std::endl;
    parser.execute(&tmp[0], tmp.size());
    if (parser.hasError()) {
        std::cout << "parser execute fail" << std::endl;
    } else {
        sylar::http::HttpRequest::ptr req = parser.getData();
        std::cout << req->toString() << std::endl;
    }
}

void test_response(const char *str) {
    sylar::http::HttpResponseParser parser;
    std::string tmp = str;
    std::cout << "<test_response>:" << std::endl
              << tmp << std::endl;
    parser.execute(&tmp[0], tmp.size());
    if (parser.hasError()) {
        std::cout << "parser execue fail" << std::endl;
    } else {
        sylar::http::HttpResponse::ptr rsp = parser.getData();
        std::cout << rsp->toString() << std::endl;
    }
}

int main(int argc, char *argv[]) {
    sylar::EnvMgr::GetInstance()->init(argc, argv);
    sylar::Config::LoadFromConfDir(sylar::EnvMgr::GetInstance()->getConfigPath());

    test_request(test_request_data);
    test_request(test_request_chunked_data);

    test_response(test_response_data);

    return 0;
}