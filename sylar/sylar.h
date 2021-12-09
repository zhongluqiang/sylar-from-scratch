/**
 * @file sylar.h
 * @brief sylar all-in-one头文件，用于外部调用，本目录下的文件绝不应该包含这个头文件
 * @version 0.1
 * @date 2021-06-08
 *
 * @mainpage sylar C++高性能分布式服务器框架
 * <table>
 * <tr> <th>Project    <td>sylar
 * <tr> <th>Author     <td>zhongluqiang
 * <tr> <th>Source     <td>https://github.com/zhongluqiang/sylar-from-scratch
 * 
 * @section 项目描述 项目描述
 * C++高性能分布式服务器框架,webserver,websocket server,自定义tcp_server（包含日志模块，配置模块，线程模块，协程模块，协程调度模块，io协程调度模块，hook模块，socket模块，bytearray序列化，http模块，TcpServer模块，Websocket模块，Https模块等, Smtp邮件模块, MySQL, SQLite3, ORM,Redis,Zookeeper)
 * 
 * @section 日志模块 日志模块
 * 支持流式日志风格写日志和格式化风格写日志，支持日志格式自定义，日志级别，多日志分离等等功能 流式日志使用：SYLAR_LOG_INFO(g_logger) << "this is a log"; 格式化日志使用：SYLAR_LOG_FMT_INFO(g_logger, "%s", "this is a log"); 支持时间,线程id,线程名称,日志级别,日志名称,文件名,行号等内容的自由配置
 * 
 */

#ifndef __SYLAR_SYLAR_H__
#define __SYLAR_SYLAR_H__

#include "log.h"
#include "util.h"
#include "singleton.h"
#include "mutex.h"
#include "noncopyable.h"
#include "macro.h"
#include "env.h"
#include "config.h"
#include "thread.h"
#include "fiber.h"
#include "scheduler.h"
#include "iomanager.h"
#include "fd_manager.h"
#include "hook.h"
#include "endian.h"
#include "address.h"
#include "socket.h"
#include "bytearray.h"
#include "tcp_server.h"
#include "uri.h"
#include "http/http.h"
#include "http/http_parser.h"
#include "http/http_session.h"
#include "http/servlet.h"
#include "http/http_server.h"
#include "http/http_connection.h"
#include "daemon.h"
#endif