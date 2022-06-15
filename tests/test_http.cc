#include "deepblue.h"

void test_request()
{
    deepblue::http::HttpRequest::ptr req(new deepblue::http::HttpRequest);
    req->setHeader("host", "www.sylar.top");
    req->setBody("hello world");

    req->dump(std::cout) << std::endl;
}

void test_response()
{
    deepblue::http::HttpResponse::ptr res(new deepblue::http::HttpResponse);
    res->setHeader("X-X", "zhangy");
    res->setBody("hello world");
    res->setStatus((deepblue::http::HttpStatus)400);
    res->setClose(false);

    res->dump(std::cout) << std::endl;
}

int main(int argc, char **argv)
{
    test_request();
    test_response();
    return 0;
}