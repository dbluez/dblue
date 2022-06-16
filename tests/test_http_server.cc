#include "deepblue.h"

static deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

void run()
{
    deepblue::http::HttpServer::ptr server(new deepblue::http::HttpServer);
    deepblue::Address::ptr addr = deepblue::Address::LookupAnyIPAddress("0.0.0.0:10001");
    while (!server->bind(addr))
    {
        sleep(2);
    }

    auto sd = server->getServletDispatch();
    sd->addServlet("/dblue/v1", [](deepblue::http::HttpRequest::ptr req,
                                   deepblue::http::HttpResponse::ptr rsp,
                                   deepblue::http::HttpSession::ptr session)
                   {
                        rsp->setBody(req->toString());
                        return 0; });

    sd->addGlobServlet("/dblue/*", [](deepblue::http::HttpRequest::ptr req,
                                      deepblue::http::HttpResponse::ptr rsp,
                                      deepblue::http::HttpSession::ptr session)
                       {
                            rsp->setBody("Glob:\r\n" + req->toString());
                            return 0; });

    server->start();
}

int main(int argc, char **argv)
{
    deepblue::IOManager iom(2);
    iom.schedule(run);
    return 0;
}