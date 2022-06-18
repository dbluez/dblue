#include "deepblue.h"

static deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

void test_pool()
{
    deepblue::http::HttpConnectionPool::ptr pool(new deepblue::http::HttpConnectionPool(
        "www.sylar.top", "", 80, 10, 1000 * 30, 5));

    deepblue::IOManager::GetThis()->addTimer(
        1000, [pool]()
        {
            auto r = pool->doGet("/", 300);
            DEEPBLUE_LOG_INFO(g_logger) << r->toString(); },
        true);
}

void run()
{
    deepblue::Address::ptr addr = deepblue::Address::LookupAnyIPAddress("www.sylar.top:80");
    if (!addr)
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "get addr failed";
        return;
    }

    deepblue::Socket::ptr sock = deepblue::Socket::CreateTCP(addr);
    bool rt = sock->connect(addr);
    if (!rt)
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "connect to addr:" << *addr << " failed!";
        return;
    }

    deepblue::http::HttpConnection::ptr conn(new deepblue::http::HttpConnection(sock));
    deepblue::http::HttpRequest::ptr req(new deepblue::http::HttpRequest);
    req->setHeader("host", "www.sylar.top");
    req->setPath("/blog/");
    DEEPBLUE_LOG_INFO(g_logger) << "req: " << std::endl
                                << *req;

    conn->sendRequest(req);
    auto rsp = conn->recvResponse();
    if (!rsp)
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "recv response error";
        return;
    }

    DEEPBLUE_LOG_INFO(g_logger) << "rsp: " << std::endl
                                << *rsp;

    // std::ofstream ofs("rsp.dat");
    // ofs << *rsp;

    DEEPBLUE_LOG_INFO(g_logger) << "============================================";

    auto r = deepblue::http::HttpConnection::DoGet("http://www.sylar.top/blog/", 300);
    DEEPBLUE_LOG_INFO(g_logger) << "result=" << r->result
                                << "error=" << r->error
                                << "rsp=" << (rsp ? rsp->toString() : "");

    DEEPBLUE_LOG_INFO(g_logger) << "============================================";
    test_pool();
}

int main(int argc, char **argv)
{

    deepblue::IOManager iom;
    iom.schedule(run);
    return 0;
}