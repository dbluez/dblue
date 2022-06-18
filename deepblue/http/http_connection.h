#ifndef __DEEPBLUE_HTTP_CONNECTION_H__
#define __DEEPBLUE_HTTP_CONNECTION_H__

#include "../streams/socket_stream.h"
#include "http.h"
#include "thread.h"
#include "uri.h"

#include <list>

namespace deepblue
{
    namespace http
    {
        // HTTP响应结果
        struct HttpResult
        {
            typedef std::shared_ptr<HttpResult> ptr;

            enum class Error
            {
                OK = 0,                      // 正常
                INVALID_URL = 1,             // 非法URL
                INVALID_HOST = 2,            // 无法解析HOST
                CONNECT_FAIL = 3,            // 连接失败
                SEND_CLOSE_BY_PEER = 4,      // 连接被对端关闭
                SEND_SOCKET_ERROR = 5,       // 发送请求产生Socket错误
                TIMEOUT = 6,                 // 超时
                CREATE_SOCKET_ERROR = 7,     // 创建Socket失败
                POOL_GET_CONNECTION = 8,     // 从连接池中取连接失败
                POOL_INVALID_CONNECTION = 9, // 无效的连接
            };

            // 构造函数
            HttpResult(int _result, HttpResponse::ptr _response, const std::string &_error)
                : result(_result),
                  response(_response),
                  error(_error)
            {
            }

            // 转字符串
            std::string toString() const;

            int result;                 // 错误码
            HttpResponse::ptr response; // HTTP响应结构体
            std::string error;          // 错误描述
        };

        class HttpConnectionPool;

        class HttpConnection : public SocketStream
        {
            friend class HttpConnectionPool;

        public:
            // HTTP客户端类智能指针
            typedef std::shared_ptr<HttpConnection> ptr;

            // 构造函数
            // sock: Socket类
            // owner: 是否掌握所有权
            HttpConnection(Socket::ptr sock, bool owner = true);

            // 析构函数
            ~HttpConnection();

            /// 发送HTTP的GET请求
            // url： 请求的url
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            static HttpResult::ptr DoGet(const std::string &url, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            // 发送HTTP的GET请求
            // uri： URI结构体
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            static HttpResult::ptr DoGet(Uri::ptr uri, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            /// 发送HTTP的POST请求
            // url： 请求的url
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            static HttpResult::ptr DoPost(const std::string &url, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            /// 发送HTTP的POST请求
            // uri： URI结构体
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            static HttpResult::ptr DoPost(Uri::ptr uri, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            // 发送HTTP请求
            // method： 请求类型
            // uri： 请求的url
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            static HttpResult::ptr DoRequest(HttpMethod method, const std::string &url, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            // 发送HTTP请求
            // method： 请求类型
            // uri： URI结构体
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            static HttpResult::ptr DoRequest(HttpMethod method, Uri::ptr uri, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            // 发送HTTP请求
            // req： 请求结构体
            // timeout_ms： 超时时间(毫秒)
            // 返回值：返回HTTP结果结构体
            static HttpResult::ptr DoRequest(HttpRequest::ptr req, Uri::ptr uri, uint64_t timeout_ms);

            // 接收HTTP响应
            HttpResponse::ptr recvResponse();

            /// 发送HTTP请求
            int sendRequest(HttpRequest::ptr req);

        private:
            uint64_t m_createTime = 0; // 创建时间
            uint64_t m_request = 0;    // 该连接已使用的次数，只在使用连接池的情况下有用
        };

        class HttpConnectionPool
        {
        public:
            typedef std::shared_ptr<HttpConnectionPool> ptr;
            typedef Mutex MutexType;

            // 构建HTTP请求池
            // host： 请求头中的Host字段默认值
            // vhost: 请求头中的Host字段默认值，vhost存在时优先使用vhost
            // port: 端口
            // max_size: 暂未使用
            // max_alive_time: 单个连接的最大存活时间
            // max_request: 单个连接可复用的最大次数
            HttpConnectionPool(const std::string &host,
                               const std::string &vhost,
                               uint32_t port,
                               uint32_t max_size,
                               uint32_t max_alive_time,
                               uint32_t max_request);

            // 从请求池中获取一个连接
            // 如果没有可用的连接，则会新建一个连接并加入到池，
            // 每次从池中取连接时，都会刷新一遍连接池，把超时的和已达到复用次数上限的连接删除
            HttpConnection::ptr getConnection();

            /// 发送HTTP的GET请求
            // url： 请求的url
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            HttpResult::ptr doGet(const std::string &url, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            // 发送HTTP的GET请求
            // uri： URI结构体
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            HttpResult::ptr doGet(Uri::ptr uri, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            /// 发送HTTP的POST请求
            // url： 请求的url
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            HttpResult::ptr doPost(const std::string &url, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            /// 发送HTTP的POST请求
            // uri： URI结构体
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            HttpResult::ptr doPost(Uri::ptr uri, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            // 发送HTTP请求
            // method： 请求类型
            // uri： 请求的url
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            HttpResult::ptr doRequest(HttpMethod method, const std::string &url, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            // 发送HTTP请求
            // method： 请求类型
            // uri： URI结构体
            // timeout_ms： 超时时间(毫秒)
            // headers： HTTP请求头部参数
            // body： 请求消息体
            // 返回值： 返回HTTP结果结构体
            HttpResult::ptr doRequest(HttpMethod method, Uri::ptr uri, uint64_t timeout_ms, const std::map<std::string, std::string> &headers = {}, const std::string &body = "");

            // 发送HTTP请求
            // req： 请求结构体
            // timeout_ms： 超时时间(毫秒)
            // 返回值：返回HTTP结果结构体
            HttpResult::ptr doRequest(HttpRequest::ptr req, uint64_t timeout_ms);

        private:
            static void ReleasePtr(HttpConnection *ptr, HttpConnectionPool *pool);

        private:
            std::string m_host;                  // Host字段默认值
            std::string m_vhost;                 // Host字段默认值，m_vhost优先级高于m_host
            uint32_t m_port;                     // 端口
            uint32_t m_maxSize;                  // 暂未使用
            uint32_t m_maxAliveTime;             // 单个连接的最大存活时间
            uint32_t m_maxRequest;               // 单个连接的最大复用次数
            MutexType m_mutex;                   // 互斥锁
            std::list<HttpConnection *> m_conns; // 连接池，链表形式存储
            std::atomic<int32_t> m_total = {0};  // 当前连接池的可用连接数量
        };

    }
}

#endif