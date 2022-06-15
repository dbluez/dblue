#include "deepblue.h"

static deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

void test_socket()
{
    deepblue::IPAddress::ptr addr = deepblue::Address::LookupAnyIPAddress("www.baidu.com");
    if (addr)
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "get address: " << addr->toString();
    }
    else
    {
        DEEPBLUE_LOG_INFO(g_logger) << "get address failed";
        return;
    }

    deepblue::Socket::ptr sock = deepblue::Socket::CreateTCP(addr);
    addr->setPort(80);
    if (!sock->connect(addr))
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "connect : " << addr->toString() << " failed!";
        return;
    }
    else
    {
        DEEPBLUE_LOG_INFO(g_logger) << "connect : " << addr->toString() << " success!";
    }

    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if (rt <= 0)
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "send failed rt =  " << rt;
        return;
    }
    else
    {
        DEEPBLUE_LOG_INFO(g_logger) << "send success rt =  " << rt;
    }

    std::string buffs;
    buffs.resize(4096);
    rt = sock->recv(&buffs[0], buffs.size());
    if (rt <= 0)
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "recv failed rt =  " << rt;
        return;
    }
    else
    {
        buffs.resize(rt);
        DEEPBLUE_LOG_INFO(g_logger) << "recv success rt =  " << rt << std::endl
                                    << " buffs: " << std::endl
                                    << buffs;
    }
}

int main(int argc, char **argv)
{
    deepblue::IOManager iom;
    iom.schedule(&test_socket);
    return 0;
}