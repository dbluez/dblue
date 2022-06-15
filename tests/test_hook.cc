#include "deepblue.h"

deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

void test_sleep()
{
    deepblue::IOManager iom(1);

    iom.schedule([]()
                 {
        sleep(2);
        DEEPBLUE_LOG_INFO(g_logger)<<"sleep 2"; });

    iom.schedule([]()
                 {
        sleep(3);
        DEEPBLUE_LOG_INFO(g_logger)<<"sleep 3"; });

    DEEPBLUE_LOG_INFO(g_logger) << "test_sleep";
}

void test_sock()
{

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "110.242.68.3", &addr.sin_addr.s_addr);

    DEEPBLUE_LOG_INFO(g_logger) << "connect begin";
    int rt = connect(sock, (const sockaddr *)&addr, sizeof(addr));
    DEEPBLUE_LOG_INFO(g_logger) << "connect rt = " << rt << " error = " << errno;

    if (rt)
    {
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    DEEPBLUE_LOG_INFO(g_logger) << "send rt = " << rt << " error = " << errno;

    if (rt < 0)
    {
        return;
    }

    std::string buf;
    buf.resize(4096);

    rt = recv(sock, &buf[0], buf.size(), 0);
    DEEPBLUE_LOG_INFO(g_logger) << "recv rt = " << rt << " error = " << errno;

    if (rt < 0)
    {
        return;
    }

    buf.resize(rt);
    DEEPBLUE_LOG_INFO(g_logger) << buf;
}

int main(int argc, char **argv)
{
    // test_sleep();
    // test_sock();

    deepblue::IOManager iom;
    iom.schedule(test_sock);
    return 0;
}