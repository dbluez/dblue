#ifndef __DEEPBLUE_TCP_SERVER_H__
#define __DEEPBLUE_TCP_SERVER_H__

#include "address.h"
#include "config.h"
#include "iomanager.h"
#include "noncopyable.h"
#include "socket.h"
#include <functional>
#include <memory>

namespace deepblue
{
    // TCP服务器封装
    class TcpServer : public std::enable_shared_from_this<TcpServer>, Noncopyable
    {
    public:
        typedef std::shared_ptr<TcpServer> ptr;

        // 构造函数
        // io_worker : socket客户端工作的协程调度器
        // accept_worker : 服务器socket执行接收socket连接的协程调度器
        TcpServer(deepblue::IOManager *io_worker = deepblue::IOManager::GetThis(),
                  deepblue::IOManager *accept_worker = deepblue::IOManager::GetThis());

        // 析构函数
        virtual ~TcpServer();

        // 绑定地址
        virtual bool bind(deepblue::Address::ptr addr);

        // 绑定地址数组
        // addrs 需要绑定的地址数组
        // fails 绑定失败的地址
        // 返回值： 是否绑定成功
        virtual bool bind(const std::vector<Address::ptr> &addrs, std::vector<Address::ptr> &fails);

        // 启动服务 (需要bind成功后执行)
        virtual bool start();

        // 停止服务
        virtual void stop();

        std::vector<Socket::ptr> getSocks() const { return m_socks; }

        // 返回读取超时时间(毫秒)
        uint64_t getRecvTimeout() const { return m_recvTimeout; }

        // 返回服务器名称
        std::string getName() const { return m_name; }

        // 设置读取超时时间(毫秒)
        void setRecvTimeout(uint64_t v) { m_recvTimeout = v; }

        // 设置服务器名称
        virtual void setName(const std::string &v) { m_name = v; }

        // 是否停止
        bool isStop() const { return m_isStop; }

        // 以字符串形式dump server信息
        virtual std::string toString(const std::string &prefix = "");

    protected:
        // 处理新连接的Socket类
        virtual void handleClient(Socket::ptr client);

        // 开始接受连接
        virtual void startAccept(Socket::ptr sock);

    protected:
        std::vector<Socket::ptr> m_socks; // 监听Socket数组
        IOManager *m_ioWorker;            // 新连接的Socket工作的调度器
        IOManager *m_acceptWorker;        // 服务器Socket接收连接的调度器
        uint64_t m_recvTimeout;           // 接收超时时间
        std::string m_name;               // 服务器名称
        std::string m_type = "tcp";       // 服务器类型
        bool m_isStop;                    // 服务是否停止
    };

}

#endif