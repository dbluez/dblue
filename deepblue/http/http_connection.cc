#include "http_connection.h"
#include "http_parser.h"
#include "log.h"

namespace deepblue
{
    namespace http
    {
        deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_NAME("system");

        std::string HttpResult::toString() const
        {
            std::stringstream ss;
            ss << "[HttpResult result=" << result
               << " error=" << error
               << " response=" << (response ? response->toString() : "nullptr")
               << "]";
            return ss.str();
        }

        // 构造函数
        // sock: Socket类
        // owner: 是否掌握所有权
        HttpConnection::HttpConnection(Socket::ptr sock, bool owner)
            : SocketStream(sock, owner)
        {
        }

        // 析构函数
        HttpConnection::~HttpConnection()
        {
            DEEPBLUE_LOG_DEBUG(g_logger) << "HttpConnection::~HttpConnection";
        }

        // 接收HTTP响应
        HttpResponse::ptr HttpConnection::recvResponse()
        {
            HttpResponseParser::ptr parser(new HttpResponseParser);
            uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();
            std::shared_ptr<char> buffer(new char[buff_size + 1], [](char *ptr)
                                         { delete[] ptr; });
            char *data = buffer.get();
            int offset = 0;
            do
            {
                int len = read(data + offset, buff_size - offset);
                if (len <= 0)
                {
                    close();
                    return nullptr;
                }
                len += offset;
                data[len] = '\0';
                size_t nparse = parser->execute(data, len);
                if (parser->hasError())
                {
                    close();
                    return nullptr;
                }
                offset = len - nparse;
                if (offset == (int)buff_size)
                {
                    close();
                    return nullptr;
                }
                if (parser->isFinished())
                {
                    break;
                }
            } while (true);

            return parser->getData();
        }

        /// 发送HTTP请求
        int HttpConnection::sendRequest(HttpRequest::ptr rsp)
        {
            std::stringstream ss;
            ss << *rsp;
            std::string data = ss.str();
            return writeFixSize(data.c_str(), data.size());
        }

        /// 发送HTTP的GET请求
        // url： 请求的url
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnection::DoGet(const std::string &url,
                                              uint64_t timeout_ms,
                                              const std::map<std::string, std::string> &headers,
                                              const std::string &body)
        {
            Uri::ptr uri = Uri::Create(url);
            if (!uri)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::INVALID_URL, nullptr, "invalid url: " + url);
            }
            return DoGet(uri, timeout_ms, headers, body);
        }

        // 发送HTTP的GET请求
        // uri： URI结构体
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnection::DoGet(Uri::ptr uri,
                                              uint64_t timeout_ms,
                                              const std::map<std::string, std::string> &headers,
                                              const std::string &body)
        {
            return DoRequest(HttpMethod::GET, uri, timeout_ms, headers, body);
        }

        /// 发送HTTP的POST请求
        // url： 请求的url
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnection::DoPost(const std::string &url,
                                               uint64_t timeout_ms,
                                               const std::map<std::string, std::string> &headers,
                                               const std::string &body)
        {
            Uri::ptr uri = Uri::Create(url);
            if (!uri)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::INVALID_URL, nullptr, "invalid url: " + url);
            }
            return DoPost(uri, timeout_ms, headers, body);
        }

        /// 发送HTTP的POST请求
        // uri： URI结构体
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnection::DoPost(Uri::ptr uri,
                                               uint64_t timeout_ms,
                                               const std::map<std::string, std::string> &headers,
                                               const std::string &body)
        {
            return DoRequest(HttpMethod::POST, uri, timeout_ms, headers, body);
        }

        // 发送HTTP请求
        // method： 请求类型
        // uri： 请求的url
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnection::DoRequest(HttpMethod method,
                                                  const std::string &url,
                                                  uint64_t timeout_ms,
                                                  const std::map<std::string, std::string> &headers,
                                                  const std::string &body)
        {
            Uri::ptr uri = Uri::Create(url);
            if (!uri)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::INVALID_URL, nullptr, "invalid url: " + url);
            }
            return DoRequest(method, uri, timeout_ms, headers, body);
        }

        // 发送HTTP请求
        // method： 请求类型
        // uri： URI结构体
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnection::DoRequest(HttpMethod method,
                                                  Uri::ptr uri,
                                                  uint64_t timeout_ms,
                                                  const std::map<std::string, std::string> &headers,
                                                  const std::string &body)
        {
            HttpRequest::ptr req = std::make_shared<HttpRequest>();
            req->setPath(uri->getPath());
            req->setQuery(uri->getQuery());
            req->setFragment(uri->getFragment());
            req->setMethod(method);
            bool has_host = false;
            for (auto &i : headers)
            {
                if (strcasecmp(i.first.c_str(), "connection") == 0)
                {
                    if (strcasecmp(i.second.c_str(), "keep-alive") == 0)
                    {
                        req->setClose(false);
                    }
                    continue;
                }
                if (!has_host && strcasecmp(i.first.c_str(), "host") == 0)
                {
                    has_host = !i.second.empty();
                }
                req->setHeader(i.first, i.second);
            }
            if (!has_host)
            {
                req->setHeader("Host", uri->getHost());
            }
            req->setBody(body);
            return DoRequest(req, uri, timeout_ms);
        }

        // 发送HTTP请求
        // req： 请求结构体
        // timeout_ms： 超时时间(毫秒)
        // 返回值：返回HTTP结果结构体
        HttpResult::ptr HttpConnection::DoRequest(HttpRequest::ptr req,
                                                  Uri::ptr uri,
                                                  uint64_t timeout_ms)
        {
            Address::ptr addr = uri->createAddress();
            if (!addr)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::INVALID_HOST, nullptr, "invalid host: " + uri->getHost());
            }

            Socket::ptr sock = Socket::CreateTCP(addr);
            if (!sock)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::CREATE_SOCKET_ERROR, nullptr, "create socket fail: " + addr->toString() + " errno=" + std::to_string(errno) + " errstr=" + std::string(strerror(errno)));
            }
            if (!sock->connect(addr))
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::CONNECT_FAIL, nullptr, "connect fail: " + addr->toString());
            }

            sock->setRecvTimeout(timeout_ms);

            HttpConnection::ptr conn = std::make_shared<HttpConnection>(sock);
            int rt = conn->sendRequest(req);
            if (rt == 0)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::SEND_CLOSE_BY_PEER, nullptr, "send request closed by peer: " + addr->toString());
            }
            if (rt < 0)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::SEND_SOCKET_ERROR, nullptr, "send request socket error errno=" + std::to_string(errno) + " errstr=" + std::string(strerror(errno)));
            }

            auto rsp = conn->recvResponse();
            if (!rsp)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::TIMEOUT, nullptr, "recv response timeout: " + addr->toString() + " timeout_ms:" + std::to_string(timeout_ms));
            }

            return std::make_shared<HttpResult>((int)HttpResult::Error::OK, rsp, "ok");
        }

        // 构建HTTP请求池
        // host： 请求头中的Host字段默认值
        // vhost: 请求头中的Host字段默认值，vhost存在时优先使用vhost
        // port: 端口
        // max_size: 暂未使用
        // max_alive_time: 单个连接的最大存活时间
        // max_request: 单个连接可复用的最大次数
        HttpConnectionPool::HttpConnectionPool(const std::string &host, const std::string &vhost, uint32_t port, uint32_t max_size, uint32_t max_alive_time, uint32_t max_request)
            : m_host(host),
              m_vhost(vhost),
              m_port(port),
              m_maxSize(max_size),
              m_maxAliveTime(max_alive_time),
              m_maxRequest(max_request)
        {
        }

        // 从请求池中获取一个连接
        // 如果没有可用的连接，则会新建一个连接并加入到池，
        // 每次从池中取连接时，都会刷新一遍连接池，把超时的和已达到复用次数上限的连接删除
        HttpConnection::ptr HttpConnectionPool::getConnection()
        {
            uint64_t now_ms = deepblue::GetCurrentMS();
            std::vector<HttpConnection *> invalid_conns;
            HttpConnection *ptr = nullptr;
            MutexType::Lock lock(m_mutex);
            while (!m_conns.empty())
            {
                auto conn = *m_conns.begin();
                m_conns.pop_front();
                if (!conn->isConnected())
                {
                    invalid_conns.push_back(conn);
                    continue;
                }
                if ((conn->m_createTime + m_maxAliveTime) > now_ms)
                {
                    invalid_conns.push_back(conn);
                    continue;
                }
                ptr = conn;
                break;
            }
            lock.unlock();
            for (auto &i : invalid_conns)
            {
                delete i;
            }
            m_total -= invalid_conns.size();

            if (!ptr)
            {
                IPAddress::ptr addr = Address::LookupAnyIPAddress(m_host);
                if (!addr)
                {
                    DEEPBLUE_LOG_ERROR(g_logger) << "get addr fail: " << m_host;
                    return nullptr;
                }
                addr->setPort(m_port);
                Socket::ptr sock = Socket::CreateTCP(addr);
                if (!sock)
                {
                    DEEPBLUE_LOG_ERROR(g_logger) << "create sock fail: " << *addr;
                    return nullptr;
                }
                if (!sock->connect(addr))
                {
                    DEEPBLUE_LOG_ERROR(g_logger) << "sock connect fail: " << *addr;
                    return nullptr;
                }

                ptr = new HttpConnection(sock);
                ++m_total;
            }
            return HttpConnection::ptr(ptr, std::bind(&HttpConnectionPool::ReleasePtr, std::placeholders::_1, this));
        }

        void HttpConnectionPool::ReleasePtr(HttpConnection *ptr,
                                            HttpConnectionPool *pool)
        {
            ++ptr->m_request;
            if (!ptr->isConnected() || ((ptr->m_createTime + pool->m_maxAliveTime) >= deepblue::GetCurrentMS()) || (ptr->m_request >= pool->m_maxRequest))
            {
                delete ptr;
                --pool->m_total;
                return;
            }
            MutexType::Lock lock(pool->m_mutex);
            pool->m_conns.push_back(ptr);
        }

        /// 发送HTTP的GET请求
        // url： 请求的url
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnectionPool::doGet(const std::string &url,
                                                  uint64_t timeout_ms,
                                                  const std::map<std::string, std::string> &headers,
                                                  const std::string &body)
        {
            return doRequest(HttpMethod::GET, url, timeout_ms, headers, body);
        }

        // 发送HTTP的GET请求
        // uri： URI结构体
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnectionPool::doGet(Uri::ptr uri,
                                                  uint64_t timeout_ms,
                                                  const std::map<std::string, std::string> &headers,
                                                  const std::string &body)
        {
            std::stringstream ss;
            ss << uri->getPath()
               << (uri->getQuery().empty() ? "" : "?")
               << uri->getQuery()
               << (uri->getFragment().empty() ? "" : "#")
               << uri->getFragment();
            return doGet(ss.str(), timeout_ms, headers, body);
        }

        /// 发送HTTP的POST请求
        // url： 请求的url
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnectionPool::doPost(const std::string &url,
                                                   uint64_t timeout_ms,
                                                   const std::map<std::string, std::string> &headers,
                                                   const std::string &body)
        {
            return doRequest(HttpMethod::POST, url, timeout_ms, headers, body);
        }

        /// 发送HTTP的POST请求
        // uri： URI结构体
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnectionPool::doPost(Uri::ptr uri,
                                                   uint64_t timeout_ms,
                                                   const std::map<std::string, std::string> &headers,
                                                   const std::string &body)
        {
            std::stringstream ss;
            ss << uri->getPath()
               << (uri->getQuery().empty() ? "" : "?")
               << uri->getQuery()
               << (uri->getFragment().empty() ? "" : "#")
               << uri->getFragment();
            return doPost(ss.str(), timeout_ms, headers, body);
        }

        // 发送HTTP请求
        // method： 请求类型
        // uri： 请求的url
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method,
                                                      const std::string &url,
                                                      uint64_t timeout_ms,
                                                      const std::map<std::string, std::string> &headers,
                                                      const std::string &body)
        {
            HttpRequest::ptr req = std::make_shared<HttpRequest>();
            req->setPath(url);
            req->setMethod(method);
            // req->setClose(false);
            bool has_host = false;
            for (auto &i : headers)
            {
                if (strcasecmp(i.first.c_str(), "connection") == 0)
                {
                    if (strcasecmp(i.second.c_str(), "keep-alive") == 0)
                    {
                        req->setClose(false);
                    }
                    continue;
                }

                if (!has_host && strcasecmp(i.first.c_str(), "host") == 0)
                {
                    has_host = !i.second.empty();
                }

                req->setHeader(i.first, i.second);
            }
            if (!has_host)
            {
                if (m_vhost.empty())
                {
                    req->setHeader("Host", m_host);
                }
                else
                {
                    req->setHeader("Host", m_vhost);
                }
            }
            req->setBody(body);
            return doRequest(req, timeout_ms);
        }

        // 发送HTTP请求
        // method： 请求类型
        // uri： URI结构体
        // timeout_ms： 超时时间(毫秒)
        // headers： HTTP请求头部参数
        // body： 请求消息体
        // 返回值： 返回HTTP结果结构体
        HttpResult::ptr HttpConnectionPool::doRequest(HttpMethod method,
                                                      Uri::ptr uri,
                                                      uint64_t timeout_ms,
                                                      const std::map<std::string, std::string> &headers,
                                                      const std::string &body)
        {
            std::stringstream ss;
            ss << uri->getPath()
               << (uri->getQuery().empty() ? "" : "?")
               << uri->getQuery()
               << (uri->getFragment().empty() ? "" : "#")
               << uri->getFragment();
            return doRequest(method, ss.str(), timeout_ms, headers, body);
        }

        // 发送HTTP请求
        // req： 请求结构体
        // timeout_ms： 超时时间(毫秒)
        // 返回值：返回HTTP结果结构体
        HttpResult::ptr HttpConnectionPool::doRequest(HttpRequest::ptr req,
                                                      uint64_t timeout_ms)
        {
            auto conn = getConnection();
            if (!conn)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::POOL_GET_CONNECTION, nullptr, "pool host:" + m_host + " port:" + std::to_string(m_port));
            }
            auto sock = conn->getSocket();
            if (!sock)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::POOL_INVALID_CONNECTION, nullptr, "pool host:" + m_host + " port:" + std::to_string(m_port));
            }
            sock->setRecvTimeout(timeout_ms);
            int rt = conn->sendRequest(req);
            if (rt == 0)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::SEND_CLOSE_BY_PEER, nullptr, "send request closed by peer: " + sock->getRemoteAddress()->toString());
            }
            if (rt < 0)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::SEND_SOCKET_ERROR, nullptr, "send request socket error errno=" + std::to_string(errno) + " errstr=" + std::string(strerror(errno)));
            }
            auto rsp = conn->recvResponse();
            if (!rsp)
            {
                return std::make_shared<HttpResult>((int)HttpResult::Error::TIMEOUT, nullptr, "recv response timeout: " + sock->getRemoteAddress()->toString() + " timeout_ms:" + std::to_string(timeout_ms));
            }
            return std::make_shared<HttpResult>((int)HttpResult::Error::OK, rsp, "ok");
        }

    }
}