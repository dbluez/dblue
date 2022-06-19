#include "application.h"
#include "config.h"
#include "env.h"
#include "log.h"
#include "daemon.h"

namespace deepblue
{
    static deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_NAME("system");

    static deepblue::ConfigVar<std::string>::ptr g_server_work_path =
        deepblue::Config::Lookup("server.work_path", std::string("/apps/work/dblue"), "server work path");

    static deepblue::ConfigVar<std::string>::ptr g_server_pid_file =
        deepblue::Config::Lookup("server.pid_file", std::string("dblue.pid"), "server pid file");

    static deepblue::ConfigVar<std::vector<TcpServerConf>>::ptr g_servers_conf =
        deepblue::Config::Lookup("servers", std::vector<TcpServerConf>(), "http server config");

    Application *Application::s_instance = nullptr;

    Application::Application()
    {
        s_instance = this;
    }

    bool Application::init(int argc, char **argv)
    {
        m_argc = argc;
        m_argv = argv;

        deepblue::EnvMgr::GetInstance()->addHelp("s", "start with the terminal");
        deepblue::EnvMgr::GetInstance()->addHelp("d", "run as daemon");
        deepblue::EnvMgr::GetInstance()->addHelp("c", "conf path default: ./conf");
        deepblue::EnvMgr::GetInstance()->addHelp("p", "print help");

        if (!deepblue::EnvMgr::GetInstance()->init(argc, argv))
        {
            deepblue::EnvMgr::GetInstance()->printHelp();
            return false;
        }

        if (deepblue::EnvMgr::GetInstance()->has("p"))
        {
            deepblue::EnvMgr::GetInstance()->printHelp();
            return false;
        }

        int run_type = 0;
        if (deepblue::EnvMgr::GetInstance()->has("s"))
        {
            run_type = 1;
        }

        if (deepblue::EnvMgr::GetInstance()->has("d"))
        {
            run_type = 2;
        }

        if (run_type == 0)
        {
            deepblue::EnvMgr::GetInstance()->printHelp();
            return false;
        }

        std::string pidfile = g_server_work_path->getValue() +
                              "/" + g_server_pid_file->getValue();

        if (deepblue::FSUtil::IsRunningPidfile(pidfile))
        {
            DEEPBLUE_LOG_ERROR(g_logger) << "server is running : " << pidfile;
            return false;
        }

        std::string conf_path = deepblue::EnvMgr::GetInstance()->getAbsolutePath(
            deepblue::EnvMgr::GetInstance()->get("c", "conf"));
        DEEPBLUE_LOG_ERROR(g_logger) << "load conf path : " << conf_path;
        deepblue::Config::LoadFromConfDir(conf_path);

        if (!deepblue::FSUtil::Mkdir(g_server_work_path->getValue()))
        {
            DEEPBLUE_LOG_ERROR(g_logger) << "create work path [" << g_server_work_path->getValue()
                                         << " error = " << errno << " errstr = " << strerror(errno);
            return false;
        }

        return true;
    }

    bool Application::run()
    {
        bool is_daemon = deepblue::EnvMgr::GetInstance()->has("d");
        return start_daemon(m_argc, m_argv,
                            std::bind(&Application::main, this, std::placeholders::_1,
                                      std::placeholders::_2),
                            is_daemon);
    }

    int Application::main(int argc, char **argv)
    {
        DEEPBLUE_LOG_INFO(g_logger) << "main";
        std::string pidfile = g_server_work_path->getValue() + "/" + g_server_pid_file->getValue();
        std::ofstream ofs(pidfile);
        if (!ofs)
        {
            DEEPBLUE_LOG_ERROR(g_logger) << "open pidfile " << pidfile << " failed!";
            return false;
        }
        ofs << getpid();

        auto http_conf = g_servers_conf->getValue();
        for (auto &i : http_conf)
        {
            DEEPBLUE_LOG_INFO(g_logger) << LexicalCast<deepblue::TcpServerConf, std::string>()(i);
        }
        return 0;

        m_mainIOManager.reset(new deepblue::IOManager(1, true, "main"));
        m_mainIOManager->schedule(std::bind(&Application::run_fiber, this));
        m_mainIOManager->addTimer(
            2000, []() {}, true); // DEEPBLUE_LOG_INFO(g_logger) << "hello";
        m_mainIOManager->stop();
        return 0;
    }

    int Application::run_fiber()
    {
        return true;
    }
}