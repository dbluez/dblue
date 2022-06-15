#ifndef __DEEPBLUE_HTTP_HTTP_SERVER_H__
#define __DEEPBLUE_HTTP_HTTP_SERVER_H__

#include "http_session.h"
#include "servlet.h"
#include "tcp_server.h"

namespace deepblue
{
    namespace http
    {

        // HTTP服务器类
        class HttpServer : public TcpServer
        {
        public:
            /// 智能指针类型
            typedef std::shared_ptr<HttpServer> ptr;

            // 构造函数
            // keepalive 是否长连接
            // worker 工作调度器
            // accept_worker 接收连接调度器
            HttpServer(bool keepalive = false,
                       deepblue::IOManager *worker = deepblue::IOManager::GetThis(),
                       deepblue::IOManager *io_worker = deepblue::IOManager::GetThis(),
                       deepblue::IOManager *accept_worker = deepblue::IOManager::GetThis());

            // 获取ServletDispatch
            ServletDispatch::ptr getServletDispatch() const { return m_dispatch; }

            // 设置ServletDispatch
            void setServletDispatch(ServletDispatch::ptr v) { m_dispatch = v; }

            virtual void setName(const std::string &v) override;

        protected:
            virtual void handleClient(Socket::ptr client) override;

        private:
            bool m_isKeepalive;              // 是否支持长连接
            ServletDispatch::ptr m_dispatch; // Servlet分发器
        };

    } // namespace http
} // namespace deepblue

#endif
