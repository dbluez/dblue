#include "deepblue.h"

static deepblue::Logger::ptr g_logger = DEEPBLUE_LOG_ROOT();

deepblue::Timer::ptr timer;
int server_main(int argc, char **argv)
{
    DEEPBLUE_LOG_INFO(g_logger) << deepblue::ProcessInfoMgr::GetInstance()->toString();
    deepblue::IOManager iom(1);
    timer = iom.addTimer(
        1000, []()
        {
            DEEPBLUE_LOG_INFO(g_logger) << "onTimer";
            static int count = 0;
            if (++count > 10) {
                exit(1);
            } },
        true);
    return 0;
}

int main(int argc, char **argv)
{
    return deepblue::start_daemon(argc, argv, server_main, argc != 1);
}
