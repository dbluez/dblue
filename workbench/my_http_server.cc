#include "../http/http_server.h"
#include "log.h"

deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

void run()
{
    deepblue::Address::ptr addr = deepblue::Address::LookupAnyIPAddress("0.0.0.0:10002");
    if (!addr)
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "get addr failed";
        return;
    }

    deepblue::http::HttpServer::ptr http_server(new deepblue::http::HttpServer);
    while (!http_server->bind(addr))
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "bind " << *addr << " failed";
        return;
    }

    http_server->start();
}

int main(int argc, char **argv)
{
    deepblue::IOManager iom(1);
    iom.schedule(run);
    return 0;
}