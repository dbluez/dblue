#include "tcp_server.h"
#include "config.h"
#include "log.h"

namespace deepblue
{
    static deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_NAME("systme");

    static deepblue::ConfigVar<uint64_t>::ptr g_tcp_server_read_timeout =
        deepblue::Config::Lookup("tcp_server.read_timeout",
                                 (uint64_t)(60 * 1000 * 2),
                                 "tcp server read timeout");

    // 构造函数
    // io_worker : socket客户端工作的协程调度器
    // accept_worker : 服务器socket执行接收socket连接的协程调度器
    TcpServer::TcpServer(deepblue::IOManager *io_worker,
                         deepblue::IOManager *accept_worker)
        : m_ioWorker(io_worker),
          m_acceptWorker(accept_worker),
          m_recvTimeout(g_tcp_server_read_timeout->getValue()),
          m_name("deepblue/1.0.0"),
          m_type("tcp"),
          m_isStop(true)
    {
    }

    // 析构函数
    TcpServer::~TcpServer()
    {
        for (auto &i : m_socks)
        {
            i->close();
        }
        m_socks.clear();
    }

    // 绑定地址
    bool TcpServer::bind(deepblue::Address::ptr addr)
    {
        std::vector<Address::ptr> addrs;
        std::vector<Address::ptr> fails;
        addrs.push_back(addr);
        return bind(addrs, fails);
    }

    // 绑定地址数组
    // addrs 需要绑定的地址数组
    // fails 绑定失败的地址
    // 返回值： 是否绑定成功
    bool TcpServer::bind(const std::vector<Address::ptr> &addrs, std::vector<Address::ptr> &fails)
    {
        for (auto &addr : addrs)
        {
            Socket::ptr sock = Socket::CreateTCP(addr);
            if (!sock->bind(addr))
            {
                DEEPBLUE_LOG_ERROR(g_logger) << "bind fail errno="
                                             << errno << " errstr=" << strerror(errno)
                                             << " addr=[" << addr->toString() << "]";
                fails.push_back(addr);
                continue;
            }
            if (!sock->listen())
            {
                DEEPBLUE_LOG_ERROR(g_logger) << "listen fail errno="
                                             << errno << " errstr=" << strerror(errno)
                                             << " addr=[" << addr->toString() << "]";
                fails.push_back(addr);
                continue;
            }
            m_socks.push_back(sock);
        }

        if (!fails.empty())
        {
            m_socks.clear();
            return false;
        }

        for (auto &i : m_socks)
        {
            DEEPBLUE_LOG_INFO(g_logger) << "type=" << m_type
                                        << " name=" << m_name
                                        << " server bind success: " << *i;
        }
        return true;
    }

    bool TcpServer::start()
    {
        if (!m_isStop)
        {
            return true;
        }
        m_isStop = false;
        for (auto &sock : m_socks)
        {
            m_acceptWorker->schedule(std::bind(&TcpServer::startAccept,
                                               shared_from_this(),
                                               sock));
        }
        return true;
    }

    void TcpServer::stop()
    {
        m_isStop = true;
        auto self = shared_from_this();
        m_acceptWorker->schedule([this, self]()
                                 {
            for(auto &sock:m_socks)
            {
                sock->cancelAll();
                sock->close();
            } 
            m_socks.clear(); });
    }

    // 处理新连接的Socket类
    void TcpServer::handleClient(Socket::ptr client)
    {
        DEEPBLUE_LOG_INFO(g_logger) << "handleClient: " << *client;
    }

    // 开始接受连接
    void TcpServer::startAccept(Socket::ptr sock)
    {
        while (!m_isStop)
        {
            Socket::ptr client = sock->accept();
            if (client)
            {
                client->setRecvTimeout(m_recvTimeout);
                m_ioWorker->schedule(std::bind(&TcpServer::handleClient,
                                               shared_from_this(), client));
            }
            else
            {
                DEEPBLUE_LOG_ERROR(g_logger) << "accept errno=" << errno
                                             << " errstr=" << strerror(errno);
            }
        }
    }

    // 以字符串形式dump server信息
    std::string TcpServer::toString(const std::string &prefix)
    {
        std::stringstream ss;
        ss << prefix << "[type=" << m_type
           << " name=" << m_name
           << " io_worker=" << (m_ioWorker ? m_ioWorker->getName() : "")
           << " accept=" << (m_acceptWorker ? m_acceptWorker->getName() : "")
           << " recv_timeout=" << m_recvTimeout << "]" << std::endl;
        std::string pfx = prefix.empty() ? "    " : prefix;
        for (auto &i : m_socks)
        {
            ss << pfx << pfx << *i << std::endl;
        }
        return ss.str();
    }
}