#include <iostream>
#include "log.h"
#include "util.h"

int main(int argc, char **argv)
{
    deepblue::Logger::ptr logger(new deepblue::Logger);
    logger->addAppender(deepblue::LogAppender::ptr(new deepblue::StdoutLogAppender));

    deepblue::FileLogAppender::ptr file_appender(new deepblue::FileLogAppender("./log.txt"));
    deepblue::LogFormatter::ptr fmt(new deepblue::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(deepblue::LogLevel::ERROR);

    logger->addAppender(file_appender);

    // deepblue::LogEvent::ptr event(new deepblue::LogEvent(__FILE__, __LINE__, 0, deepblue::GetThreadId(), deepblue::GetFiberId(), time(0)));
    // event->getSS() << "hello deepblue log";
    // logger->log(deepblue::LogLevel::DEBUG, event);
    std::cout << "hello deepblue log" << std::endl;

    DEEPBLUE_LOG_INFO(logger) << "test macro";
    DEEPBLUE_LOG_ERROR(logger) << "test macro error";

    DEEPBLUE_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = deepblue::LoggerMgr::GetInstance()->getLogger("xx");
    DEEPBLUE_LOG_INFO(l) << "xxx";
    return 0;
}
