#ifndef __DEEPBLUE_HTTP_SERVLET_H__
#define __DEEPBLUE_HTTP_SERVLET_H__

#include "http.h"
#include "http_session.h"
#include "thread.h"
#include "util.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace deepblue
{
    namespace http
    {

        // Servlet封装
        class Servlet
        {
        public:
            // 智能指针类型定义
            typedef std::shared_ptr<Servlet> ptr;

            // 构造函数
            Servlet(const std::string &name) : m_name(name) {}

            // 析构函数
            virtual ~Servlet() {}

            // 处理请求
            // request ：HTTP请求
            // response ：HTTP响应
            // session ：HTTP连接
            // 返回值：是否处理成功
            virtual int32_t handle(deepblue::http::HttpRequest::ptr request,
                                   deepblue::http::HttpResponse::ptr response,
                                   deepblue::http::HttpSession::ptr session) = 0;

            // 返回Servlet名称
            const std::string &getName() const { return m_name; }

        protected:
            std::string m_name; // 名称
        };

        // 函数式Servlet
        class FunctionServlet : public Servlet
        {
        public:
            // 智能指针类型定义
            typedef std::shared_ptr<FunctionServlet> ptr;

            // 函数回调类型定义
            typedef std::function<int32_t(deepblue::http::HttpRequest::ptr request,
                                          deepblue::http::HttpResponse::ptr response,
                                          deepblue::http::HttpSession::ptr session)>
                callback;

            // 构造函数
            FunctionServlet(callback cb);

            virtual int32_t handle(deepblue::http::HttpRequest::ptr request,
                                   deepblue::http::HttpResponse::ptr response,
                                   deepblue::http::HttpSession::ptr session) override;

        private:
            callback m_cb; // 回调函数
        };

        class IServletCreator
        {
        public:
            typedef std::shared_ptr<IServletCreator> ptr;
            virtual ~IServletCreator() {}
            virtual Servlet::ptr get() const = 0;
            virtual std::string getName() const = 0;
        };

        class HoldServletCreator : public IServletCreator
        {
        public:
            typedef std::shared_ptr<HoldServletCreator> ptr;
            HoldServletCreator(Servlet::ptr slt)
                : m_servlet(slt)
            {
            }

            Servlet::ptr get() const override
            {
                return m_servlet;
            }

            std::string getName() const override
            {
                return m_servlet->getName();
            }

        private:
            Servlet::ptr m_servlet;
        };

        template <class T>
        class ServletCreator : public IServletCreator
        {
        public:
            typedef std::shared_ptr<ServletCreator> ptr;

            ServletCreator()
            {
            }

            Servlet::ptr get() const override
            {
                return Servlet::ptr(new T);
            }

            std::string getName() const override
            {
                return TypeToName<T>();
            }
        };

        // Servlet分发器
        class ServletDispatch : public Servlet
        {
        public:
            // 智能指针类型定义
            typedef std::shared_ptr<ServletDispatch> ptr;

            // 读写锁类型定义
            typedef RWMutex RWMutexType;

            // 构造函数
            ServletDispatch();

            virtual int32_t handle(deepblue::http::HttpRequest::ptr request,
                                   deepblue::http::HttpResponse::ptr response,
                                   deepblue::http::HttpSession::ptr session) override;

            // 添加servlet
            // uri： uri
            // slt： serlvet
            void addServlet(const std::string &uri, Servlet::ptr slt);

            // 添加servlet
            // uri：uri
            // cb：FunctionServlet回调函数
            void addServlet(const std::string &uri, FunctionServlet::callback cb);

            // 添加模糊匹配servlet
            // uri：uri 模糊匹配 /deepblue_*
            // slt：servlet
            void addGlobServlet(const std::string &uri, Servlet::ptr slt);

            // 添加模糊匹配servlet
            // uri：uri 模糊匹配 /deepblue_*
            // cb：FunctionServlet回调函数
            void addGlobServlet(const std::string &uri, FunctionServlet::callback cb);

            void addServletCreator(const std::string &uri, IServletCreator::ptr creator);

            void addGlobServletCreator(const std::string &uri, IServletCreator::ptr creator);

            template <class T>
            void addServletCreator(const std::string &uri)
            {
                addServletCreator(uri, std::make_shared<ServletCreator<T>>());
            }

            template <class T>
            void addGlobServletCreator(const std::string &uri)
            {
                addGlobServletCreator(uri, std::make_shared<ServletCreator<T>>());
            }

            // 删除servlet
            void delServlet(const std::string &uri);

            // 删除模糊匹配servlet
            void delGlobServlet(const std::string &uri);

            // 返回默认servlet
            Servlet::ptr getDefault() const { return m_default; }

            // 设置默认servlet
            void setDefault(Servlet::ptr v) { m_default = v; }

            // 通过uri获取servlet
            Servlet::ptr getServlet(const std::string &uri);

            // 通过uri获取模糊匹配servlet
            Servlet::ptr getGlobServlet(const std::string &uri);

            // 通过uri获取servlet
            // 优先精准匹配,其次模糊匹配,最后返回默认
            Servlet::ptr getMatchedServlet(const std::string &uri);

            void listAllServletCreator(std::map<std::string, IServletCreator::ptr> &infos);
            void listAllGlobServletCreator(std::map<std::string, IServletCreator::ptr> &infos);

        private:
            RWMutexType m_mutex; // 读写互斥量

            // 精准匹配servlet MAP
            // uri(/deepblue/xxx) -> servlet
            std::unordered_map<std::string, IServletCreator::ptr> m_datas;

            // 模糊匹配servlet 数组
            // uri(/deepblue/*) -> servlet
            std::vector<std::pair<std::string, IServletCreator::ptr>> m_globs;

            Servlet::ptr m_default; // 默认servlet，所有路径都没匹配到时使用
        };

        // NotFoundServlet(默认返回404)
        class NotFoundServlet : public Servlet
        {
        public:
            // 智能指针类型定义
            typedef std::shared_ptr<NotFoundServlet> ptr;

            // 构造函数
            NotFoundServlet(const std::string &name);

            virtual int32_t handle(deepblue::http::HttpRequest::ptr request,
                                   deepblue::http::HttpResponse::ptr response,
                                   deepblue::http::HttpSession::ptr session) override;

        private:
            std::string m_name;
            std::string m_content;
        };

    } // namespace http
} // namespace deepblue

#endif
