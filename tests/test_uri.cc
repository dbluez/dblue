#include "uri.h"
#include <iostream>

int main(int argc, char **argv)
{
    // deepblue::Uri::ptr uri = deepblue::Uri::Create("http://www.sylar.top/test/uri?id=100&name=deepblue#frg");
    // deepblue::Uri::ptr uri = deepblue::Uri::Create("http://admin@www.sylar.top/test/中文/uri?id=100&name=deepblue&vv=中文#frg中文");
    // deepblue::Uri::ptr uri = deepblue::Uri::Create("http://admin@www.sylar.top");
    // deepblue::Uri::ptr uri = deepblue::Uri::Create("http://www.sylar.top/test/uri");
    auto uri = deepblue::Uri::Create("http://a:b@host.com:8080/p/a/t/h?query=string#hash");

    std::cout << uri->toString() << std::endl;
    auto addr = uri->createAddress();
    std::cout << *addr << std::endl;
    return 0;
}
