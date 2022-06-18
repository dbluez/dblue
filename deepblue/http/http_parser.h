#ifndef __DEEPBLUE_HTTP_PARSER_H__
#define __DEEPBLUE_HTTP_PARSER_H__

#include "http.h"

namespace deepblue
{
    namespace http
    {
        class HttpRequestParser
        {
        public:
            // HTTP解析类的智能指针
            typedef std::shared_ptr<HttpRequestParser> ptr;

            // 构造函数
            HttpRequestParser();

            // 解析HTTP响应协议  data:协议数据内存, len:协议数据内存大小
            // (返回实际解析的长度, 并且将已解析的数据移除)
            size_t execute(char *data, size_t len);

            // 是否解析完成
            int isFinished() const { return m_finished; }

            // 设置是否解析完成
            void setFinished(bool v) { m_finished = v; }

            // 是否有错误
            int hasError() const { return !!m_error; }

            // 设置错误
            void setError(int v) { m_error = v; }

            // 返回HttpRequest结构体
            HttpRequest::ptr getData() const { return m_data; }

            // 获取http_parser结构体
            const http_parser &getParser() const { return m_parser; }

            // 获取当前的HTTP头部field
            const std::string &getField() const { return m_field; }

            // 设置当前的HTTP头部field
            void setField(const std::string &v) { m_field = v; }

            // 获取消息体长度
            uint64_t getContentLength();

        public:
            // 返回HttpRequest协议解析的缓存大小
            static uint64_t GetHttpRequestBufferSize();

            // 返回HttpRequest协议的最大消息体大小
            static uint64_t GetHttpRequestMaxBodySize();

        private:
            http_parser m_parser;    // http_parser
            HttpRequest::ptr m_data; // HttpRequest
            int m_error;             // 错误码，参考http_errno
            bool m_finished;         // 是否解析结束
            std::string m_field;     // 当前的HTTP头部field，http-parser解析HTTP头部是field和value分两次返回
        };

        class HttpResponseParser
        {
        public:
            // 智能指针类型
            typedef std::shared_ptr<HttpResponseParser> ptr;

            // 构造函数
            HttpResponseParser();

            // 解析HTTP响应协议  data:协议数据内存, len:协议数据内存大小
            // (返回实际解析的长度, 并且将已解析的数据移除)
            size_t execute(char *data, size_t len);

            // 是否解析完成
            int isFinished() const { return m_finished; }

            // 设置是否解析完成
            void setFinished(bool v) { m_finished = v; }

            // 是否有错误
            int hasError() const { return !!m_error; }

            // 设置错误
            void setError(int v) { m_error = v; }

            // 返回HttpRequest结构体
            HttpResponse::ptr getData() const { return m_data; }

            // 获取http_parser结构体
            const http_parser &getParser() const { return m_parser; }

            // 获取当前的HTTP头部field
            const std::string &getField() const { return m_field; }

            // 设置当前的HTTP头部field
            void setField(const std::string &v) { m_field = v; }

            // 获取消息体长度
            uint64_t getContentLength();

        public:
            // 返回HttpRequest协议解析的缓存大小
            static uint64_t GetHttpResponseBufferSize();

            // 返回HttpRequest协议的最大消息体大小
            static uint64_t GetHttpResponseMaxBodySize();

        private:
            http_parser m_parser;     // HTTP响应解析器
            HttpResponse::ptr m_data; // HTTP响应对象
            int m_error;              // 错误码
            bool m_finished;          // 是否解析结束
            std::string m_field;      // 当前的HTTP头部field
        };
    }
}

#endif