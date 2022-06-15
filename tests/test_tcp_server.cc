#include "tcp_server.h"

deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

void run()
{

    auto addr = deepblue::Address::LookupAny("0.0.0.0:10001");
    std::vector<deepblue::Address::ptr> addrs;
    addrs.push_back(addr);

    deepblue::TcpServer::ptr tcp_server(new deepblue::TcpServer);
    std::vector<deepblue::Address::ptr> fails;
    while (!tcp_server->bind(addrs, fails))
    {
        sleep(2);
    }
    tcp_server->start();
}

int main(int argc, char **argv)
{

    deepblue::IOManager iom(2);
    iom.schedule(run);
    return 0;
}