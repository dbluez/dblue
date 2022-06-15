#include "address.h"
#include "log.h"

deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

void test_look()
{
    std::vector<deepblue::Address::ptr> addrs;

    bool v = deepblue::Address::Lookup(addrs, "www.baidu.com:ftp");
    if (!v)
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "lookup failed";
        return;
    }

    for (size_t i = 0; i < addrs.size(); ++i)
    {
        DEEPBLUE_LOG_INFO(g_logger) << i << " - " << addrs[i]->toString();
    }
}

void test_iface()
{
    std::multimap<std::string, std::pair<deepblue::Address::ptr, uint32_t>> results;
    bool v = deepblue::Address::GetInterfaceAddresses(results, AF_INET6);
    if (!v)
    {
        DEEPBLUE_LOG_ERROR(g_logger) << "GetInterfaceAddresses failed";
        return;
    }

    for (auto &i : results)
    {
        DEEPBLUE_LOG_INFO(g_logger) << i.first << " - " << i.second.first->toString()
                                    << " - " << i.second.second;
    }
}

void test_ipv4()
{
    auto addr = deepblue::IPAddress::Create("127.0.0.8");
    if (addr)
    {
        DEEPBLUE_LOG_INFO(g_logger) << addr->toString();
    }
}

int main(int argc, char **argv)
{
    // test_look();
    test_iface();
    // test_ipv4();
    return 0;
}