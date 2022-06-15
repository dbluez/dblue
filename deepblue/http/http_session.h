#ifndef __DEEPBLUE_HTTP_SESSION_H__
#define __DEEPBLUE_HTTP_SESSION_H__

#include "../streams/socket_stream.h"
#include "http.h"

namespace deepblue
{
    namespace http
    {

        // HTTPSession封装
        class HttpSession : public SocketStream
        {
        public:
            /// 智能指针类型定义
            typedef std::shared_ptr<HttpSession> ptr;

            // 构造函数
            // sock Socket类型
            // owner 是否托管
            HttpSession(Socket::ptr sock, bool owner = true);

            // 接收HTTP请求
            HttpRequest::ptr recvRequest();

            // 发送HTTP响应
            int sendResponse(HttpResponse::ptr rsp);
        };

    } // namespace http
} // namespace deepblue

#endif
