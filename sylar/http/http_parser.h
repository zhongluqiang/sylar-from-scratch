/**
 * @file http_parser.h
 * @brief HTTP协议解析封装
 * @version 0.1
 * @date 2021-09-25
 */
#ifndef __SYLAR_HTTP_PARSER_H__
#define __SYLAR_HTTP_PARSER_H__

#include "http.h"

namespace sylar {
namespace http {

/**
 * @brief HTTP请求解析类
 */
class HttpRequestParser {
public:
    /// HTTP解析类的智能指针
    typedef std::shared_ptr<HttpRequestParser> ptr;

    /**
     * @brief 构造函数
     */
    HttpRequestParser();

    /**
     * @brief 解析协议
     * @param[in, out] data 协议文本内存
     * @param[in] len 协议文本内存长度
     * @return 返回实际解析的长度,并且将已解析的数据移除
     */
    size_t execute(char *data, size_t len);

    /**
     * @brief 是否解析完成
     * @return 是否解析完成
     */
    int isFinished() const { return m_finished; }

    /**
     * @brief 设置是否解析完成
     */
    void setFinished(bool v) { m_finished = v; }

    /**
     * @brief 是否有错误
     * @return 是否有错误
     */
    int hasError() const { return !!m_error; }

    /**
     * @brief 设置错误
     * @param[in] v 错误值
     */
    void setError(int v) { m_error = v; }

    /**
     * @brief 返回HttpRequest结构体
     */
    HttpRequest::ptr getData() const { return m_data; }

    /**
     * @brief 获取http_parser结构体
     */
    const http_parser &getParser() const { return m_parser; }

    /**
     * @brief 获取当前的HTTP头部field
     */
    const std::string &getField() const { return m_field; }

    /**
     * @brief 设置当前的HTTP头部field
     */
    void setField(const std::string &v) { m_field = v; }

public:
    /**
     * @brief 返回HttpRequest协议解析的缓存大小
     */
    static uint64_t GetHttpRequestBufferSize();

    /**
     * @brief 返回HttpRequest协议的最大消息体大小
     */
    static uint64_t GetHttpRequestMaxBodySize();

private:
    /// http_parser
    http_parser m_parser;
    /// HttpRequest
    HttpRequest::ptr m_data;
    /// 错误码，参考http_errno
    int m_error;
    /// 是否解析结束
    bool m_finished;
    /// 当前的HTTP头部field，http-parser解析HTTP头部是field和value分两次返回
    std::string m_field;
};

/**
 * @brief Http响应解析结构体
 */
class HttpResponseParser {
public:
    /// 智能指针类型
    typedef std::shared_ptr<HttpResponseParser> ptr;

    /**
     * @brief 构造函数
     */
    HttpResponseParser();

    /**
     * @brief 解析HTTP响应协议
     * @param[in, out] data 协议数据内存
     * @param[in] len 协议数据内存大小
     * @param[in] chunck 是否在解析chunck
     * @return 返回实际解析的长度,并且移除已解析的数据
     */
    size_t execute(char *data, size_t len);

    /**
     * @brief 是否解析完成
     */
    int isFinished() const { return m_finished; }

    /**
     * @brief 设置是否解析完成
     */
    void setFinished(bool v) { m_finished = v; }

    /**
     * @brief 是否有错误
     */
    int hasError() const { return !!m_error; }

    /**
     * @brief 设置错误码
     * @param[in] v 错误码
     */
    void setError(int v) { m_error = v; }

    /**
     * @brief 返回HttpResponse
     */
    HttpResponse::ptr getData() const { return m_data; }

    /**
     * @brief 返回http_parser
     */
    const http_parser &getParser() const { return m_parser; }

    /**
     * @brief 获取当前的HTTP头部field
     */
    const std::string &getField() const { return m_field; }

    /**
     * @brief 设置当前的HTTP头部field
     */
    void setField(const std::string &v) { m_field = v; }

public:
    /**
     * @brief 返回HTTP响应解析缓存大小
     */
    static uint64_t GetHttpResponseBufferSize();

    /**
     * @brief 返回HTTP响应最大消息体大小
     */
    static uint64_t GetHttpResponseMaxBodySize();

private:
    /// HTTP响应解析器
    http_parser m_parser;
    /// HTTP响应对象
    HttpResponse::ptr m_data;
    /// 错误码
    int m_error;
    /// 是否解析结束
    bool m_finished;
    /// 当前的HTTP头部field
    std::string m_field;
};

} // namespace http
} // namespace sylar

#endif