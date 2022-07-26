/**
 * @file http_connection.h
 * @brief HTTP客户端类
 * @author sylar.yin
 * @email 564628276@qq.com
 * @date 2019-06-11
 * @copyright Copyright (c) 2019年 sylar.yin All rights reserved (www.sylar.top)
 */

#ifndef __SYLAR_HTTP_CONNECTION_H__
#define __SYLAR_HTTP_CONNECTION_H__

#include "../streams/socket_stream.h"
#include "http.h"
#include "../uri.h"
#include "../thread.h"

#include <list>

namespace sylar {
namespace http {

/**
 * @brief HTTP响应结果
 */
struct HttpResult {
    /// 智能指针类型定义
    typedef std::shared_ptr<HttpResult> ptr;
    /**
     * @brief 错误码定义
     */
    enum class Error {
        /// 正常
        OK = 0,
        /// 非法URL
        INVALID_URL = 1,
        /// 无法解析HOST
        INVALID_HOST = 2,
        /// 连接失败
        CONNECT_FAIL = 3,
        /// 连接被对端关闭
        SEND_CLOSE_BY_PEER = 4,
        /// 发送请求产生Socket错误
        SEND_SOCKET_ERROR = 5,
        /// 超时
        TIMEOUT = 6,
        /// 创建Socket失败
        CREATE_SOCKET_ERROR = 7,
        /// 从连接池中取连接失败
        POOL_GET_CONNECTION = 8,
        /// 无效的连接
        POOL_INVALID_CONNECTION = 9,
    };

    /**
     * @brief 构造函数
     * @param[in] _result 错误码
     * @param[in] _response HTTP响应结构体
     * @param[in] _error 错误描述
     */
    HttpResult(int _result
               ,HttpResponse::ptr _response
               ,const std::string& _error)
        :result(_result)
        ,response(_response)
        ,error(_error) {}

    /// 错误码
    int result;
    /// HTTP响应结构体
    HttpResponse::ptr response;
    /// 错误描述
    std::string error;
    /// 转字符串
    std::string toString() const;
};

class HttpConnectionPool;
/**
 * @brief HTTP客户端类
 */
class HttpConnection : public SocketStream {
friend class HttpConnectionPool;
public:
    /// HTTP客户端类智能指针
    typedef std::shared_ptr<HttpConnection> ptr;

    /**
     * @brief 发送HTTP的GET请求
     * @param[in] url 请求的url
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    static HttpResult::ptr DoGet(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP的GET请求
     * @param[in] uri URI结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    static HttpResult::ptr DoGet(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP的POST请求
     * @param[in] url 请求的url
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    static HttpResult::ptr DoPost(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP的POST请求
     * @param[in] uri URI结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    static HttpResult::ptr DoPost(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * @param[in] method 请求类型
     * @param[in] uri 请求的url
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    static HttpResult::ptr DoRequest(HttpMethod method
                            , const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * @param[in] method 请求类型
     * @param[in] uri URI结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    static HttpResult::ptr DoRequest(HttpMethod method
                            , Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * @param[in] req 请求结构体
     * @param[in] uri URI结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @return 返回HTTP结果结构体
     */
    static HttpResult::ptr DoRequest(HttpRequest::ptr req
                            , Uri::ptr uri
                            , uint64_t timeout_ms);

    /**
     * @brief 构造函数
     * @param[in] sock Socket类
     * @param[in] owner 是否掌握所有权
     */
    HttpConnection(Socket::ptr sock, bool owner = true);

    /**
     * @brief 析构函数
     */
    ~HttpConnection();

    /**
     * @brief 接收HTTP响应
     */
    HttpResponse::ptr recvResponse();

    /**
     * @brief 发送HTTP请求
     * @param[in] req HTTP请求结构
     */
    int sendRequest(HttpRequest::ptr req);

private:
    /// 创建时间
    uint64_t m_createTime = 0;
    /// 该连接已使用的次数，只在使用连接池的情况下有用
    uint64_t m_request = 0;
};

class HttpConnectionPool {
public:
    typedef std::shared_ptr<HttpConnectionPool> ptr;
    typedef Mutex MutexType;

    /**
     * @brief 构建HTTP请求池
     * @param[in] host 请求头中的Host字段默认值
     * @param[in] vhost 请求头中的Host字段默认值，vhost存在时优先使用vhost
     * @param[in] port 端口
     * @param[in] max_size 暂未使用
     * @param[in] max_alive_time 单个连接的最大存活时间
     * @param[in] max_request 单个连接可复用的最大次数
     */
    HttpConnectionPool(const std::string& host
                       ,const std::string& vhost
                       ,uint32_t port
                       ,uint32_t max_size
                       ,uint32_t max_alive_time
                       ,uint32_t max_request);

    /**
     * @brief 从请求池中获取一个连接
     * @note 如果没有可用的连接，则会新建一个连接并加入到池，
     *       每次从池中取连接时，都会刷新一遍连接池，把超时的和已达到复用次数上限的连接删除
     */
    HttpConnection::ptr getConnection();


    /**
     * @brief 发送HTTP的GET请求
     * @param[in] url 请求的url
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doGet(const std::string& url
                          , uint64_t timeout_ms
                          , const std::map<std::string, std::string>& headers = {}
                          , const std::string& body = "");

    /**
     * @brief 发送HTTP的GET请求
     * @param[in] uri URI结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doGet(Uri::ptr uri
                           , uint64_t timeout_ms
                           , const std::map<std::string, std::string>& headers = {}
                           , const std::string& body = "");

    /**
     * @brief 发送HTTP的POST请求
     * @param[in] url 请求的url
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doPost(const std::string& url
                           , uint64_t timeout_ms
                           , const std::map<std::string, std::string>& headers = {}
                           , const std::string& body = "");

    /**
     * @brief 发送HTTP的POST请求
     * @param[in] uri URI结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doPost(Uri::ptr uri
                           , uint64_t timeout_ms
                           , const std::map<std::string, std::string>& headers = {}
                           , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * @param[in] method 请求类型
     * @param[in] uri 请求的url
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doRequest(HttpMethod method
                            , const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * @param[in] method 请求类型
     * @param[in] uri URI结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @param[in] headers HTTP请求头部参数
     * @param[in] body 请求消息体
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doRequest(HttpMethod method
                            , Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    /**
     * @brief 发送HTTP请求
     * @param[in] req 请求结构体
     * @param[in] timeout_ms 超时时间(毫秒)
     * @return 返回HTTP结果结构体
     */
    HttpResult::ptr doRequest(HttpRequest::ptr req
                            , uint64_t timeout_ms);
private:
    static void ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool);
private:
    /// Host字段默认值
    std::string m_host;
    /// Host字段默认值，m_vhost优先级高于m_host
    std::string m_vhost;
    /// 端口
    uint32_t m_port;
    /// 暂未使用
    uint32_t m_maxSize;
    /// 单个连接的最大存活时间
    uint32_t m_maxAliveTime;
    /// 单个连接的最大复用次数
    uint32_t m_maxRequest;
    /// 互斥锁
    MutexType m_mutex;
    /// 连接池，链表形式存储
    std::list<HttpConnection*> m_conns;
    /// 当前连接池的可用连接数量
    std::atomic<int32_t> m_total = {0};
};

}
}

#endif
