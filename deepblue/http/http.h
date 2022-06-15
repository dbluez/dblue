#ifndef __DBLUE_HTTP_HTTP_H__
#define __DBLUE_HTTP_HTTP_H__

#include "./thirdparty/http_parser.h"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace deepblue
{
    namespace http
    {

        // HTTP方法枚举
        enum class HttpMethod
        {
#define XX(num, name, string) name = num,
            HTTP_METHOD_MAP(XX)
#undef XX
                INVALID_METHOD
        };

        // HTTP状态枚举
        enum class HttpStatus
        {
#define XX(code, name, desc) name = code,
            HTTP_STATUS_MAP(XX)
#undef XX
        };

        // 将字符串方法名转成HTTP方法枚举
        HttpMethod StringToHttpMethod(const std::string &m);

        // 将字符串指针转换成HTTP方法枚举
        HttpMethod CharsToHttpMethod(const char *m);

        // 将HTTP方法枚举转换成字符串
        const char *HttpMethodToString(const HttpMethod &m);

        // 将HTTP状态枚举转换成字符串
        const char *HttpStatusToString(const HttpStatus &s);

        // 忽略大小写比较仿函数
        struct CaseInsensitiveLess
        {
            // 忽略大小写比较字符串
            bool operator()(const std::string &lhs, const std::string &rhs) const;
        };

        /**
         * @brief 获取Map中的key值,并转成对应类型,返回是否成功
         * @param[in] m Map数据结构
         * @param[in] key 关键字
         * @param[out] val 保存转换后的值
         * @param[in] def 默认值
         * @return
         *      @retval true 转换成功, val 为对应的值
         *      @retval false 不存在或者转换失败 val = def
         */
        template <class MapType, class T>
        bool checkGetAs(const MapType &m, const std::string &key, T &val, const T &def = T())
        {
            auto it = m.find(key);
            if (it == m.end())
            {
                val = def;
                return false;
            }
            try
            {
                val = boost::lexical_cast<T>(it->second);
                return true;
            }
            catch (...)
            {
                val = def;
            }
            return false;
        }

        /**
         * @brief 获取Map中的key值,并转成对应类型
         * @param[in] m Map数据结构
         * @param[in] key 关键字
         * @param[in] def 默认值
         * @return 如果存在且转换成功返回对应的值,否则返回默认值
         */
        template <class MapType, class T>
        T getAs(const MapType &m, const std::string &key, const T &def = T())
        {
            auto it = m.find(key);
            if (it == m.end())
            {
                return def;
            }
            try
            {
                return boost::lexical_cast<T>(it->second);
            }
            catch (...)
            {
            }
            return def;
        }

        class HttpResponse;
        class HttpRequest
        {
        public:
            // HTTP请求的智能指针
            typedef std::shared_ptr<HttpRequest> ptr;
            // MAP结构
            typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;

            // 构造函数
            HttpRequest(uint8_t version = 0x11, bool close = true);

            // 从HTTP请求构造HTTP响应(只需要保证请求与响应的版本号与keep-alive一致即可)
            std::shared_ptr<HttpResponse> createResponse();

            // 返回HTTP方法
            HttpMethod getMethod() const { return m_method; }

            // 返回HTTP版本
            uint8_t getVersion() const { return m_version; }

            // 返回HTTP请求的路径
            const std::string &getPath() const { return m_path; }

            // 返回HTTP请求的查询参数
            const std::string &getQuery() const { return m_query; }

            // 返回HTTP请求的消息体
            const std::string &getBody() const { return m_body; }

            // 返回HTTP请求的消息头MAP
            const MapType &getHeaders() const { return m_headers; }

            // 返回HTTP请求的参数MAP
            const MapType &getParams() const { return m_params; }

            // 返回HTTP请求的cookie MAP
            const MapType &getCookies() const { return m_cookies; }

            // 设置HTTP请求的方法名
            void setMethod(HttpMethod v) { m_method = v; }

            // 设置响应版本
            void setVersion(uint8_t v) { m_version = v; }

            // 设置HTTP请求的路径
            void setPath(const std::string &v) { m_path = v; }

            // 设置HTTP请求的查询参数
            void setQuery(const std::string &v) { m_query = v; }

            // 设置HTTP请求的Fragment
            void setFragment(const std::string &v) { m_fragment = v; }

            // 设置HTTP请求的消息体
            void setBody(const std::string &v) { m_body = v; }

            // 追加HTTP请求的消息体
            void appendBody(const std::string &v) { m_body.append(v); }

            // 是否自动关闭
            bool isClose() const { return m_close; }

            // 设置是否自动关闭
            void setClose(bool v) { m_close = v; }

            // 是否websocket
            bool isWebsocket() const { return m_websocket; }

            // 设置是否websocket
            void setWebsocket(bool v) { m_websocket = v; }

            // 设置HTTP请求的头部MAP
            void setHeaders(const MapType &v) { m_headers = v; }

            // 设置HTTP请求的参数MAP
            void setParams(const MapType &v) { m_params = v; }

            // 设置HTTP请求的Cookie MAP
            void setCookies(const MapType &v) { m_cookies = v; }

            // 获取HTTP请求的头部参数(如果存在则返回对应值,否则返回默认值)
            std::string getHeader(const std::string &key, const std::string &def = "") const;

            // 获取HTTP请求的请求参数(如果存在则返回对应值,否则返回默认值)
            std::string getParam(const std::string &key, const std::string &def = "");

            // 获取HTTP请求的Cookie参数(如果存在则返回对应值,否则返回默认值)
            std::string getCookie(const std::string &key, const std::string &def = "");

            // 设置HTTP请求的头部参数
            void setHeader(const std::string &key, const std::string &val);

            // 设置HTTP请求的请求参数
            void setParam(const std::string &key, const std::string &val);

            // 设置HTTP请求的Cookie参数
            void setCookie(const std::string &key, const std::string &val);

            // 删除HTTP请求的头部参数
            void delHeader(const std::string &key);

            // 删除HTTP请求的请求参数
            void delParam(const std::string &key);

            //删除HTTP请求的Cookie参数
            void delCookie(const std::string &key);

            // 判断HTTP请求的头部参数是否存在
            bool hasHeader(const std::string &key, std::string *val = nullptr);

            // 判断HTTP请求的请求参数是否存在
            bool hasParam(const std::string &key, std::string *val = nullptr);

            // 判断HTTP请求的Cookie参数是否存在
            bool hasCookie(const std::string &key, std::string *val = nullptr);

            /**
             * @brief 检查并获取HTTP请求的头部参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[out] val 返回值
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回true,否则失败val=def
             */
            template <class T>
            bool checkGetHeaderAs(const std::string &key, T &val, const T &def = T())
            {
                return checkGetAs(m_headers, key, val, def);
            }

            /**
             * @brief 获取HTTP请求的头部参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回对应的值,否则返回def
             */
            template <class T>
            T getHeaderAs(const std::string &key, const T &def = T())
            {
                return getAs(m_headers, key, def);
            }

            /**
             * @brief 检查并获取HTTP请求的请求参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[out] val 返回值
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回true,否则失败val=def
             */
            template <class T>
            bool checkGetParamAs(const std::string &key, T &val, const T &def = T())
            {
                initQueryParam();
                initBodyParam();
                return checkGetAs(m_params, key, val, def);
            }

            /**
             * @brief 获取HTTP请求的请求参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回对应的值,否则返回def
             */
            template <class T>
            T getParamAs(const std::string &key, const T &def = T())
            {
                initQueryParam();
                initBodyParam();
                return getAs(m_params, key, def);
            }

            /**
             * @brief 检查并获取HTTP请求的Cookie参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[out] val 返回值
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回true,否则失败val=def
             */
            template <class T>
            bool checkGetCookieAs(const std::string &key, T &val, const T &def = T())
            {
                initCookies();
                return checkGetAs(m_cookies, key, val, def);
            }

            /**
             * @brief 获取HTTP请求的Cookie参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回对应的值,否则返回def
             */
            template <class T>
            T getCookieAs(const std::string &key, const T &def = T())
            {
                initCookies();
                return getAs(m_cookies, key, def);
            }

            // 序列化输出到流
            std::ostream &dump(std::ostream &os) const;

            // 转成字符串
            std::string toString() const;

            // 设置重定向，在头部添加Location字段，值为uri
            void setRedirect(const std::string &uri);

            // 提取url中的查询参数
            void initQueryParam();

            // 当content-type是application/x-www-form-urlencoded时，提取消息体中的表单参数
            void initBodyParam();

            // 提取请求中的cookies
            void initCookies();

            // 初始化，实际是判断connection是否为keep-alive，以设置是否自动关闭套接字
            void init();

        private:
            HttpMethod m_method;       // HTTP方法
            uint8_t m_version;         // HTTP版本
            bool m_close;              // 是否自动关闭
            bool m_websocket;          // 是否为websocket
            uint8_t m_parserParamFlag; // 参数解析标志位，0:未解析，1:已解析url参数, 2:已解析http消息体中的参数，4:已解析cookies
            std::string m_url;         // 请求的完整url
            std::string m_path;        // 请求路径
            std::string m_query;       // 请求参数
            std::string m_fragment;    // 请求fragment
            std::string m_body;        // 请求消息体
            MapType m_headers;         // 请求头部MAP
            MapType m_params;          // 请求参数MAP
            MapType m_cookies;         // 请求Cookie MAP
        };

        // HTTP响应结构体
        class HttpResponse
        {
        public:
            // HTTP响应结构智能指针
            typedef std::shared_ptr<HttpResponse> ptr;
            // MapType
            typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;

            // 构造函数 version:版本,close:是否自动关闭
            HttpResponse(uint8_t version = 0x11, bool close = true);

            // 返回响应状态
            HttpStatus getStatus() const { return m_status; }

            // 返回响应版本
            uint8_t getVersion() const { return m_version; }

            // 返回响应消息体
            const std::string &getBody() const { return m_body; }

            // 返回响应原因
            const std::string &getReason() const { return m_reason; }

            // 返回响应头部MAP
            const MapType &getHeaders() const { return m_headers; }

            // 设置响应状态
            void setStatus(HttpStatus s) { m_status = s; }

            // 设置响应版本
            void setVersion(uint8_t v) { m_version = v; }

            // 设置响应消息体
            void setBody(const std::string &v) { m_body = v; }

            // 追加HTTP请求的消息体
            void appendBody(const std::string &v) { m_body.append(v); }

            // 设置响应原因
            void setReason(const std::string &v) { m_reason = v; }

            // 设置响应头部MAP
            void setHeaders(const MapType &v) { m_headers = v; }

            // 是否自动关闭
            bool isClose() const { return m_close; }

            // 设置是否自动关闭
            void setClose(bool v) { m_close = v; }

            // 是否websocket
            bool isWebsocket() const { return m_websocket; }

            // 设置是否websocket
            void setWebsocket(bool v) { m_websocket = v; }

            // 获取响应头部参数 key:关键字, def:默认值
            std::string getHeader(const std::string &key, const std::string &def) const;

            // 设置响应头部参数
            void setHeader(const std::string &key, const std::string &val);

            // 删除响应头部参数
            void delHeader(const std::string &key);

            /**
             * @brief 检查并获取响应头部参数
             * @tparam T 值类型
             * @param[in] key 关键字
             * @param[out] val 值
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回true,否则失败val=def
             */
            template <class T>
            bool checkGetHeaderAs(const std::string &key, T &val, const T &def = T())
            {
                return checkKGetAs(m_headers, key, val, def);
            }

            /**
             * @brief 获取响应的头部参数
             * @tparam T 转换类型
             * @param[in] key 关键字
             * @param[in] def 默认值
             * @return 如果存在且转换成功返回对应的值,否则返回def
             */
            template <class T>
            T getHeaderAs(const std::string &key, const T &def = T())
            {
                return getAs(m_headers, key, def);
            }

            // 序列化输出到流
            std::ostream &dump(std::ostream &os) const;

            // 转成字符串
            std::string toString() const;

            // 设置重定向，在头部添加Location字段，值为uri
            void setRedirect(const std::string &uri);

            /**
             * @brief 为响应添加cookie
             * @param[] key cookie的key值
             * @param[] val cookie的value
             * @param[] expired 过期时间
             * @param[] path cookie的影响路径
             * @param[] domain cookie作用的域
             * @param[] secure 安全标志
             */
            void setCookie(const std::string &key, const std::string &val,
                           time_t expired = 0, const std::string &path = "",
                           const std::string &domain = "", bool secure = false);

        private:
            HttpStatus m_status;                // 响应状态
            uint8_t m_version;                  // 版本
            bool m_close;                       // 是否自动关闭
            bool m_websocket;                   // 是否为websocket
            std::string m_body;                 // 响应消息体
            std::string m_reason;               // 响应原因
            MapType m_headers;                  // 响应头部MAP
            std::vector<std::string> m_cookies; // cookies
        };

        // 流式输出HttpRequest
        std::ostream &operator<<(std::ostream &os, const HttpRequest &req);

        // 流式输出HttpResponse
        std::ostream &operator<<(std::ostream &os, const HttpResponse &rsp);

    }
}

#endif