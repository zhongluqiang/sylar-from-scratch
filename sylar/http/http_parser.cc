/**
 * @file http_parser.cc
 * @brief HTTP协议解析实现
 * @version 0.1
 * @date 2021-09-25
 */
#include "http_parser.h"
#include "../config.h"
#include "../log.h"

namespace sylar {
namespace http {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("http");

static sylar::ConfigVar<uint64_t>::ptr g_http_request_buffer_size =
    sylar::Config::Lookup("http.request.buffer_size", (uint64_t)(4 * 1024), "http request buffer size");

static sylar::ConfigVar<uint64_t>::ptr g_http_request_max_body_size =
    sylar::Config::Lookup("http.request.max_body_size", (uint64_t)(64 * 1024 * 1024), "http request max body size");

static sylar::ConfigVar<uint64_t>::ptr g_http_response_buffer_size =
    sylar::Config::Lookup("http.response.buffer_size", (uint64_t)(4 * 1024), "http response buffer size");

static sylar::ConfigVar<uint64_t>::ptr g_http_response_max_body_size =
    sylar::Config::Lookup("http.response.max_body_size", (uint64_t)(64 * 1024 * 1024), "http response max body size");

static uint64_t s_http_request_buffer_size    = 0;
static uint64_t s_http_request_max_body_size  = 0;
static uint64_t s_http_response_buffer_size   = 0;
static uint64_t s_http_response_max_body_size = 0;

uint64_t HttpRequestParser::GetHttpRequestBufferSize() {
    return s_http_request_buffer_size;
}

uint64_t HttpRequestParser::GetHttpRequestMaxBodySize() {
    return s_http_request_max_body_size;
}

uint64_t HttpResponseParser::GetHttpResponseBufferSize() {
    return s_http_response_buffer_size;
}

uint64_t HttpResponseParser::GetHttpResponseMaxBodySize() {
    return s_http_response_max_body_size;
}

namespace {
struct _RequestSizeIniter {
    _RequestSizeIniter() {
        s_http_request_buffer_size    = g_http_request_buffer_size->getValue();
        s_http_request_max_body_size  = g_http_request_max_body_size->getValue();
        s_http_response_buffer_size   = g_http_response_buffer_size->getValue();
        s_http_response_max_body_size = g_http_response_max_body_size->getValue();

        g_http_request_buffer_size->addListener(
            [](const uint64_t &ov, const uint64_t &nv) {
                s_http_request_buffer_size = nv;
            });

        g_http_request_max_body_size->addListener(
            [](const uint64_t &ov, const uint64_t &nv) {
                s_http_request_max_body_size = nv;
            });

        g_http_response_buffer_size->addListener(
            [](const uint64_t &ov, const uint64_t &nv) {
                s_http_response_buffer_size = nv;
            });

        g_http_response_max_body_size->addListener(
            [](const uint64_t &ov, const uint64_t &nv) {
                s_http_response_max_body_size = nv;
            });
    }
};
static _RequestSizeIniter _init;
} // namespace

/**
 * @brief http请求开始解析回调函数
 */
static int on_request_message_begin_cb(http_parser *p) {
    SYLAR_LOG_DEBUG(g_logger) << "on_request_message_begin_cb";
    return 0;
}

/**
 * @brief http请求头部字段解析结束，可获取头部信息字段，如method/version等
 * @note 返回0表示成功，返回1表示该HTTP消息无消息体，返回2表示无消息体并且该连接后续不会再有消息
 */
static int on_request_headers_complete_cb(http_parser *p) {
    SYLAR_LOG_DEBUG(g_logger) << "on_request_headers_complete_cb";
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(p->data);
    parser->getData()->setVersion(((p->http_major) << 0x4) | (p->http_minor));
    parser->getData()->setMethod((HttpMethod)(p->method));
    return 0;
}

/**
 * @brief http解析结束回调
 */
static int on_request_message_complete_cb(http_parser *p) {
    SYLAR_LOG_DEBUG(g_logger) << "on_request_message_complete_cb";
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(p->data);
    parser->setFinished(true);
    return 0;
}

/**
 * @brief http分段头部回调，可获取分段长度
 */
static int on_request_chunk_header_cb(http_parser *p) {
    SYLAR_LOG_DEBUG(g_logger) << "on_request_chunk_header_cb";
    return 0;
}

/**
 * @brief http分段结束回调，表示当前分段已解析完成
 */
static int on_request_chunk_complete_cb(http_parser *p) {
    SYLAR_LOG_DEBUG(g_logger) << "on_request_chunk_complete_cb";
    return 0;
}

/**
 * @brief http请求url解析完成回调
 */
static int on_request_url_cb(http_parser *p, const char *buf, size_t len) {
    SYLAR_LOG_DEBUG(g_logger) << "on_request_url_cb, url is:" << std::string(buf, len);

    int ret;
    struct http_parser_url url_parser;
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(p->data);

    http_parser_url_init(&url_parser);
    ret = http_parser_parse_url(buf, len, 0, &url_parser);
    if (ret != 0) {
        SYLAR_LOG_DEBUG(g_logger) << "parse url fail";
        return 1;
    }
    if (url_parser.field_set & (1 << UF_PATH)) {
        parser->getData()->setPath(std::string(buf + url_parser.field_data[UF_PATH].off,
                                               url_parser.field_data[UF_PATH].len));
    }
    if (url_parser.field_set & (1 << UF_QUERY)) {
        parser->getData()->setQuery(std::string(buf + url_parser.field_data[UF_QUERY].off,
                                                url_parser.field_data[UF_QUERY].len));
    }
    if (url_parser.field_set & (1 << UF_FRAGMENT)) {
        parser->getData()->setFragment(std::string(buf + url_parser.field_data[UF_FRAGMENT].off,
                                                   url_parser.field_data[UF_FRAGMENT].len));
    }
    return 0;
}

/**
 * @brief http请求首部字段名称解析完成回调
 */
static int on_request_header_field_cb(http_parser *p, const char *buf, size_t len) {
    std::string field(buf, len);
    SYLAR_LOG_DEBUG(g_logger) << "on_request_header_field_cb, field is:" << field;
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(p->data);
    parser->setField(field);
    return 0;
}

/**
 * @brief http请求首部字段值解析完成回调
 */
static int on_request_header_value_cb(http_parser *p, const char *buf, size_t len) {
    std::string value(buf, len);
    SYLAR_LOG_DEBUG(g_logger) << "on_request_header_value_cb, value is:" << value;
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(p->data);
    parser->getData()->setHeader(parser->getField(), value);
    return 0;
}

/**
 * @brief http请求响应状态回调，这个回调没有用，因为http请求不带状态
 */
static int on_request_status_cb(http_parser *p, const char *buf, size_t len) {
    SYLAR_LOG_DEBUG(g_logger) << "on_request_status_cb, should not happen";
    return 0;
}

/**
 * @brief http消息体回调
 * @note 当传输编码是chunked时，每个chunked数据段都会触发一次当前回调，所以用append的方法将所有数据组合到一起
 */
static int on_request_body_cb(http_parser *p, const char *buf, size_t len) {
    std::string body(buf, len);
    SYLAR_LOG_DEBUG(g_logger) << "on_request_body_cb, body is:" << body;
    HttpRequestParser *parser = static_cast<HttpRequestParser *>(p->data);
    parser->getData()->appendBody(body);
    return 0;
}

static http_parser_settings s_request_settings = {
    .on_message_begin    = on_request_message_begin_cb,
    .on_url              = on_request_url_cb,
    .on_status           = on_request_status_cb,
    .on_header_field     = on_request_header_field_cb,
    .on_header_value     = on_request_header_value_cb,
    .on_headers_complete = on_request_headers_complete_cb,
    .on_body             = on_request_body_cb,
    .on_message_complete = on_request_message_complete_cb,
    .on_chunk_header     = on_request_chunk_header_cb,
    .on_chunk_complete   = on_request_chunk_complete_cb};

HttpRequestParser::HttpRequestParser() {
    http_parser_init(&m_parser, HTTP_REQUEST);
    m_data.reset(new HttpRequest);
    m_parser.data = this;
    m_error       = 0;
    m_finished    = false;
}

size_t HttpRequestParser::execute(char *data, size_t len) {
    size_t nparsed = http_parser_execute(&m_parser, &s_request_settings, data, len);
    if (m_parser.upgrade) {
        //处理新协议，暂时不处理
        SYLAR_LOG_DEBUG(g_logger) << "found upgrade, ignore";
        setError(HPE_UNKNOWN);
    } else if (m_parser.http_errno != 0) {
        SYLAR_LOG_DEBUG(g_logger) << "parse request fail: " << http_errno_name(HTTP_PARSER_ERRNO(&m_parser));
        setError((int8_t)m_parser.http_errno);
    } else {
        if (nparsed < len) {
            memmove(data, data + nparsed, (len - nparsed));
        }
    }
    return nparsed;
}

/**
 * @brief http响应开始解析回调函数
 */
static int on_response_message_begin_cb(http_parser *p) {
    SYLAR_LOG_DEBUG(g_logger) << "on_response_message_begin_cb";
    return 0;
}

/**
 * @brief http响应头部字段解析结束，可获取头部信息字段，如status_code/version等
 * @note 返回0表示成功，返回1表示该HTTP消息无消息体，返回2表示无消息体并且该连接后续不会再有消息
 */
static int on_response_headers_complete_cb(http_parser *p) {
    SYLAR_LOG_DEBUG(g_logger) << "on_response_headers_complete_cb";
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(p->data);
    parser->getData()->setVersion(((p->http_major) << 0x4) | (p->http_minor));
    parser->getData()->setStatus((HttpStatus)(p->status_code));
    return 0;
}

/**
 * @brief http响应解析结束回调
 */
static int on_response_message_complete_cb(http_parser *p) {
    SYLAR_LOG_DEBUG(g_logger) << "on_response_message_complete_cb";
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(p->data);
    parser->setFinished(true);
    return 0;
}

/**
 * @brief http分段头部回调，可获取分段长度
 */
static int on_response_chunk_header_cb(http_parser *p) {
    SYLAR_LOG_DEBUG(g_logger) << "on_response_chunk_header_cb";
    return 0;
}

/**
 * @brief http分段结束回调，表示全部分段已解析完成
 */
static int on_response_chunk_complete_cb(http_parser *p) {
    SYLAR_LOG_DEBUG(g_logger) << "on_response_chunk_complete_cb";
    return 0;
}

/**
 * @brief http响应url解析完成回调，这个回调没有意义，因为响应不会携带url
 */
static int on_response_url_cb(http_parser *p, const char *buf, size_t len) {
    SYLAR_LOG_DEBUG(g_logger) << "on_response_url_cb, should not happen";
    return 0;
}

/**
 * @brief http响应首部字段名称解析完成回调
 */
static int on_response_header_field_cb(http_parser *p, const char *buf, size_t len) {
    std::string field(buf, len);
    SYLAR_LOG_DEBUG(g_logger) << "on_response_header_field_cb, field is:" << field;
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(p->data);
    parser->setField(field);
    return 0;
}

/**
 * @brief http响应首部字段值解析完成回调
 */
static int on_response_header_value_cb(http_parser *p, const char *buf, size_t len) {
    std::string value(buf, len);
    SYLAR_LOG_DEBUG(g_logger) << "on_response_header_value_cb, value is:" << value;
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(p->data);
    parser->getData()->setHeader(parser->getField(), value);
    return 0;
}

/**
 * @brief http响应状态回调
 */
static int on_response_status_cb(http_parser *p, const char *buf, size_t len) {
    SYLAR_LOG_DEBUG(g_logger) << "on_response_status_cb, status code is: " << p->status_code << ", status msg is: " << std::string(buf, len);
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(p->data);
    parser->getData()->setStatus(HttpStatus(p->status_code));
    return 0;
}

/**
 * @brief http响应消息体回调
 */
static int on_response_body_cb(http_parser *p, const char *buf, size_t len) {
    std::string body(buf, len);
    SYLAR_LOG_DEBUG(g_logger) << "on_response_body_cb, body is:" << body;
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(p->data);
    parser->getData()->appendBody(body);
    return 0;
}

static http_parser_settings s_response_settings = {
    .on_message_begin    = on_response_message_begin_cb,
    .on_url              = on_response_url_cb,
    .on_status           = on_response_status_cb,
    .on_header_field     = on_response_header_field_cb,
    .on_header_value     = on_response_header_value_cb,
    .on_headers_complete = on_response_headers_complete_cb,
    .on_body             = on_response_body_cb,
    .on_message_complete = on_response_message_complete_cb,
    .on_chunk_header     = on_response_chunk_header_cb,
    .on_chunk_complete   = on_response_chunk_complete_cb};

HttpResponseParser::HttpResponseParser() {
    http_parser_init(&m_parser, HTTP_RESPONSE);
    m_data.reset(new HttpResponse);
    m_parser.data = this;
    m_error       = 0;
    m_finished    = false;
}

size_t HttpResponseParser::execute(char *data, size_t len) {
    size_t nparsed = http_parser_execute(&m_parser, &s_response_settings, data, len);
    if (m_parser.http_errno != 0) {
        SYLAR_LOG_DEBUG(g_logger) << "parse response fail: " << http_errno_name(HTTP_PARSER_ERRNO(&m_parser));
        setError((int8_t)m_parser.http_errno);
    } else {
        if (nparsed < len) {
            memmove(data, data + nparsed, (len - nparsed));
        }
    }
    return nparsed;
}

} // namespace http
} // namespace sylar